#pragma once
#include <string.h>
#include <string>
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

inline ASTNode* make_variable_declaration(TokenType kind,
                                          const Token& name_tok,
                                          const Token& value_tok) {
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
