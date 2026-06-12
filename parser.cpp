#include <cstddef>
#include <vector>
#include <string.h>
#include <string>
#include "tree_nodes.cpp"
#include "lexer.cpp"
#include "ast_builders.h"

class Parser {
    public:
    Parser() { 
        index_token = 0; 
        start = 0;
        end = 0;
    };

    ASTNode* parse_token(Token &token) {
        ASTNode *out_node = nullptr;

        // Every statement type is dispatched in parse_statement; brackets and
        // parentheses are consumed inside the helpers (parse_block/_function/
        // _call), so they never lead a statement here.
        while (peek().kind != TokenType::END_OF_FILE) {
            ASTNode* stmt = parse_statement();
            if (stmt) out_node = stmt;   // TODO: collect into a Program body
        }

        return out_node;
    };

    private:
    std::vector<Token> tokens_to_parse;
    size_t index_token;
    size_t start;
    size_t end;


    bool is_operator(TokenType kind) {
        return kind == TokenType::PLUS  || kind == TokenType::MINUS ||
               kind == TokenType::STAR  || kind == TokenType::DIVIDE;
    }

    // Parse a single statement and return it as an ASTNode, or nullptr if the
    // leading token doesn't start a statement we understand yet.
    ASTNode* parse_statement() {
        Token     token = advance();
        TokenType kind  = token.kind;

        if (kind == TokenType::INTEGER || kind == TokenType::FLOAT ||
            kind == TokenType::BOOLEAN || kind == TokenType::CHAR) {
            Token name_tok  = advance();   // variable name
            advance();                     // '='
            Token value_tok = advance();   // value
            return make_variable_declaration(kind, name_tok, value_tok);
        }
        if (kind == TokenType::PRINT) {
            return parse_expression();
        }
        if (kind == TokenType::FUNCTION) {
            return parse_function(token);  // nested function declaration
        }
        if (kind == TokenType::RETURN) {
            ASTNode* argument = parse_expression();
            return make_ReturnStatement(argument, token);
        }
        if (kind == TokenType::UNIDENTIFIED) {
            // `foo(a)` is a call; a bare name is part of an expression.
            if (peek().kind == TokenType::PARENTESIS_OPEN) {
                return parse_call(token);
            }
            index_token--;                 // put it back; let the expression parser read it
            return parse_expression();
        }
        if (kind == TokenType::NUMBER) {
            index_token--;                 // put it back; let the expression parser read it
            return parse_expression();
        }
        return nullptr;
    }

    // Parse `function name ( params ) { body }`. Assumes the `function`
    // keyword has already been consumed and is passed in as func_tok.
    ASTNode* parse_function(Token func_tok) {
        Token id_tok = advance();          // function name
        advance();                         // consume '('

        // Collect parameters until ')'. Commas are just separators.
        std::vector<Identifier*> params;
        while (peek().kind != TokenType::PARENTESIS_CLOSE &&
               peek().kind != TokenType::END_OF_FILE) {
            Token param_tok = advance();
            if (param_tok.kind == TokenType::COMMA) continue;
            params.push_back(make_identifier(param_tok));
        }
        advance();                         // consume ')'

        Identifier*     id   = make_identifier(id_tok);
        BlockStatement* body = parse_block();   // parses '{ ... }'

        return make_FunctionDeclaration(params, /*generator*/ false,
                                        /*expression*/ false,
                                        id, body, &func_tok);
    }

    // Parse a call `name ( args )`. Assumes the callee name has already been
    // consumed and is passed in as callee_tok; peek() is the '('.
    ASTNode* parse_call(Token callee_tok) {
        ASTNode* callee = make_identifier_node(callee_tok);
        advance();                         // consume '('

        std::vector<ASTNode*> arguments;
        while (peek().kind != TokenType::PARENTESIS_CLOSE &&
               peek().kind != TokenType::END_OF_FILE) {
            if (peek().kind == TokenType::COMMA) { advance(); continue; }
            arguments.push_back(parse_expression());
        }
        Token close_tok = advance();       // consume ')'

        return make_CallExpression(callee, arguments, close_tok);
    }

    // Parse '{ ... }': consume the braces and every statement in between.
    BlockStatement* parse_block() {
        Token open_tok = advance();        // consume '{'

        std::vector<ASTNode*> body;
        while (peek().kind != TokenType::BRAKET_CLOSE &&
               peek().kind != TokenType::END_OF_FILE) {
            ASTNode* stmt = parse_statement();
            if (stmt) body.push_back(stmt);
        }

        Token close_tok = advance();       // consume '}'
        // make_blockStatement now wraps the block in an ASTNode; callers here
        // want the BlockStatement* directly, so unwrap the union arm.
        return make_blockStatement(body, open_tok, close_tok)->as.blockStatement;
    }

    // One "atom": a literal (10, 3.14) or a name (a, b).
    ASTNode* parse_primary() {
        Token tok = advance();

        if (tok.kind == TokenType::INTEGER || tok.kind == TokenType::FLOAT ||
            tok.kind == TokenType::BOOLEAN || tok.kind == TokenType::CHAR) {
            return make_literal_node(tok, tok.kind);
        }
        if (tok.kind == TokenType::UNIDENTIFIED) {
            return make_identifier_node(tok);
        }
        return nullptr; // unexpected token
    }

    // Flat, left-to-right: a + b + c -> ((a + b) + c).
    // No recursion needed yet; add precedence here when you support a + b * c.
    ASTNode* parse_expression() {
        ASTNode* left = parse_primary();

        while (is_operator(peek().kind)) {
            Token    op_tok = advance();
            Operator op     = token_to_operator(op_tok);
            ASTNode* right  = parse_primary();
            left = make_binary_node(left, op, right);
        }
        return left;
    }

    Token peek() {
        if (index_token >= tokens_to_parse.size()) return Token{TokenType::END_OF_FILE, ""};
        return tokens_to_parse[index_token];
    };

    Token advance() {
        if (index_token < tokens_to_parse.size()) index_token++;
        return tokens_to_parse[index_token - 1];
    };
};

class AST {
    public: 
};


int main() {

    std::string str = "int a = 10 int b = 2 print a + b\n function ale(b) {b*2} "; 
    Lexer lex(str);

    std::vector<Token> tokenS = lex.tokenize();
    lex.printTokens(tokenS);
    //sc.printTokenList(textList);
    return 0;
}

//output:
// 0 -> int
// 10 -> a
// 7 -> =
// 10 -> 10
// 0 -> int
// 10 -> b
// 7 -> =
// 10 -> 2
// 9 -> print
// 10 -> a
// 4 -> +
// 10 -> b
// notice each unident token has behind a type/equal... for now.....

//Scanner -> lexer -> parcer -> AST

//1 - divide text in chunks by spaces
//2 - tokenize them into Token struct
//... i think now i should turn the tokens in expresions that can be 3 types
//      -variable creation
//      -variable assignation
//      -function call

//... i also turn them into control flow statments
//      -if
//      -while
//      -for

//next build tree with the structures created

