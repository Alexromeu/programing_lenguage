#pragma once
#include <string.h>
#include <string>
#include <vector>
#include "tree_nodes.cpp"
#include "lexer.cpp"

// ============================================================
//  AST builder helpers
//  Small factory functions that allocate a node, stamp its
//  position/type, and fill its fields. They exist so the parser
//  doesn't repeat the same allocate/set boilerplate for every
//  variable type (int / float / bool / char).
// ============================================================


inline Identifier* make_identifier(const Token& name_tok) {
    Identifier* id = new Identifier();
    id->type  = "Identifier";
    id->start = name_tok.column;
    id->end   = name_tok.column + name_tok.value.size();
    id->name  = strdup(name_tok.value.c_str()); // char* needs its own copy
    return id;
}

inline Literal* make_literal(const Token& value_tok, int value) {
    Literal* lit = new Literal();
    lit->type  = "Literal";
    lit->start = value_tok.column;
    lit->end   = value_tok.column + value_tok.value.size();
    lit->value = value;
    lit->raw   = strdup(value_tok.value.c_str());
    return lit;
}

inline int decode_literal_value(TokenType kind, const std::string& raw) {
    switch (kind) {
        case TokenType::INTEGER: return std::stoi(raw);
        case TokenType::FLOAT:   return (int)std::stof(raw);     // truncated; raw keeps "3.14"
        case TokenType::BOOLEAN: return (raw == "true") ? 1 : 0;
        case TokenType::CHAR:    return raw.empty() ? 0 : raw[0]; // store the char code
        default:                 return 0;
    }
}

inline ASTNode* make_variable_declaration(TokenType kind,const Token& name_tok, const Token& value_tok) {         
    Identifier* id  = make_identifier(name_tok);
    Literal*    lit = make_literal(value_tok, decode_literal_value(kind, value_tok.value));

    // VariableDeclarator::init is an ASTNode*, so wrap the literal.
    ASTNode* init = new ASTNode();
    init->type  = "Literal";
    init->start = lit->start;
    init->end   = lit->end;
    init->as.literal = lit;

    VariableDeclarator* decl = new VariableDeclarator();
    decl->type  = "VariableDeclarator";
    decl->start = id->start;
    decl->end   = init->end;
    decl->id    = id;
    decl->init  = init;

    ASTNode* node = new ASTNode();
    node->type = "VariableDeclarator";
    node->as.variableDeclarator = decl;
    return node;
}

// Wrap a literal token (10, 3.14, true, 'c') in an ASTNode.
inline ASTNode* make_literal_node(const Token& tok, TokenType kind) {
    Literal* lit = make_literal(tok, decode_literal_value(kind, tok.value));

    ASTNode* node = new ASTNode();
    node->type  = "Literal";
    node->start = lit->start;
    node->end   = lit->end;
    node->as.literal = lit;
    return node;
}

// Wrap a name token (a, b, foo) in an ASTNode.
inline ASTNode* make_identifier_node(const Token& tok) {
    Identifier* id = make_identifier(tok);

    ASTNode* node = new ASTNode();
    node->type  = "Identifier";
    node->start = id->start;
    node->end   = id->end;
    node->as.identifier = id;
    return node;
}

// Map an operator token ("+", "-", ...) to the Operator enum.
inline Operator token_to_operator(const Token& tok) {
    auto it = StringToOperator.find(tok.value);
    return (it != StringToOperator.end()) ? it->second : Operator::Unknown;
}

// Combine two already-parsed sides into `left op right`.
inline ASTNode* make_binary_node(ASTNode* left, Operator op, ASTNode* right) {
    BinaryExpression* bin = new BinaryExpression();
    bin->type      = "BinaryExpression";
    bin->start     = left->start;
    bin->end       = right->end;
    bin->left      = left;
    bin->operator_ = op;
    bin->right     = right;

    ASTNode* node = new ASTNode();
    node->type  = "BinaryExpression";
    node->start = bin->start;
    node->end   = bin->end;
    node->as.binaryExpression = bin;
    return node;
}


inline ASTNode* make_FunctionDeclaration(std::vector<Identifier*> params, bool generator, bool expression, Identifier* id, BlockStatement* body, Token* tok) {
    FunctionDeclaration* func_decl = new FunctionDeclaration();
    func_decl->type       = "FunctionDeclaration";
    func_decl->start      = tok->column + tok->value.size();
    func_decl->end        = body->end;
    func_decl->id         = id;
    func_decl->expression = expression;
    func_decl->generator  = generator;
    func_decl->params     = params;
    func_decl->body       = body;

    ASTNode* node = new ASTNode();
    node->type  = "FunctionDeclaration";
    node->start = func_decl->start;
    node->end   = func_decl->end;
    node->as.functionDeclaration = func_decl;
    return node;
}

// Build a call like `foo2(a)`: a callee identifier plus its arguments.
inline ASTNode* make_CallExpression(ASTNode* callee,
                                    std::vector<ASTNode*> arguments,
                                    const Token& close_tok) {
    CallExpression* call = new CallExpression();
    call->type      = "CallExpression";
    call->start     = callee->start;
    call->end       = close_tok.column + close_tok.value.size();
    call->callee    = callee;
    call->arguments = arguments;
    call->optional  = false;

    ASTNode* node = new ASTNode();
    node->type  = "CallExpression";
    node->start = call->start;
    node->end   = call->end;
    node->as.callExpression = call;
    return node;
}

// Build `return <argument>`. argument is the already-parsed expression node.
inline ASTNode* make_ReturnStatement(ASTNode* argument, const Token& return_tok) {
    ReturnStatement* ret = new ReturnStatement();
    ret->type     = "ReturnStatement";
    ret->start    = return_tok.column;
    ret->end      = argument ? argument->end
                             : return_tok.column + return_tok.value.size();
    ret->argument = argument;

    ASTNode* node = new ASTNode();
    node->type  = "ReturnStatement";
    node->start = ret->start;
    node->end   = ret->end;
    node->as.returnStatement = ret;
    return node;
}

// Wrap an already-parsed list of statements between '{' and '}' in a block.
// The parser does the actual statement parsing; this just stamps the node.
inline BlockStatement* make_blockStatement(const std::vector<ASTNode*>& body,
                                           const Token& open_tok,
                                           const Token& close_tok) {
    BlockStatement* block = new BlockStatement();
    block->type  = "BlockStatement";
    block->start = open_tok.column;
    block->end   = close_tok.column + close_tok.value.size();
    block->body  = body;
    return block;
}

inline IfStatement* make_ifStatement(const Token& if_tok, BinaryExpression* test, BlockStatement* consequent, BlockStatement* alternate = nullptr) {
    IfStatement* ifstmt = new IfStatement();
    ifstmt->type = "IfStatement";
    ifstmt->test = test;
    ifstmt->consequent = consequent;
    ifstmt->alternate = alternate;

    ifstmt->start = if_tok.column;
    ifstmt->end   = alternate ? alternate->end : consequent->end;

    return ifstmt;
}