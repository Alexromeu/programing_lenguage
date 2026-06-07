#include <cstddef>
#include <vector>
#include <string.h>
#include <string>
#include "tree_nodes.cpp"
#include "lexer.cpp"
#include "ast_builders.h"



class Parser {
    //identifies the Tokens and creates structures 
    public:
    Parser() { 
        index_token = 0; 
        start = 0;
        end = 0;
    };

    ASTNode* parse_token(Token &token) {
        ASTNode *out_node = nullptr;

        while (peek().kind != TokenType::END_OF_FILE) {
            Token token = advance();
            TokenType kind = token.kind;
            std::string value = token.value;


            // int / float / bool / char all share the `type name = value`
            // shape, so one branch handles them via the AST builder helpers.
            if (kind == TokenType::INTEGER || kind == TokenType::FLOAT ||
                kind == TokenType::BOOLEAN || kind == TokenType::CHAR) {
                Token name_tok  = advance();   // the variable name, e.g. "a"
                advance();                     // EQUALS "=", discarded
                Token value_tok = advance();   // the initial value, e.g. "10"

                out_node = make_variable_declaration(kind, name_tok, value_tok);
            }

            // ==========================================
            // OPERATORS
            // ==========================================
            else if (kind == TokenType::PLUS) {
                // Handled when token is '+'
            } 
            else if (kind == TokenType::MINUS) {
                // Handled when token is '-'
            } 
            else if (kind == TokenType::STAR) {
                // Handled when token is '*'
            } 
            else if (kind == TokenType::DIVIDE) {
                // Handled when token is '/'
            } 
            else if (kind == TokenType::EQUALS) {
                // Handled when token is '='
            } 

            // ==========================================
            // INTERNAL / SYSTEM NAMES
            // ==========================================
            else if (kind == TokenType::PRINT) {
                // Handled when token is 'printOut'
            } 
            else if (kind == TokenType::FUNCTION) {
                // Handled when token is 'function'
            } 
            else if (kind == TokenType::UNIDENTIFIED) {
                // Handled when token is a variable name, number literal, etc.
            } 

            // ==========================================
            // CONTROL FLOW
            // ==========================================
            else if (kind == TokenType::IF) {
                // Handled when token is 'if'
            } 
            else if (kind == TokenType::ELSE) {
                // Handled when token is 'else'
            } 
            else if (kind == TokenType::WHILE) {
                // Handled when token is 'while'
            } 
            else if (kind == TokenType::FOR) {
                // Handled when token is 'for'
            } 

            // ==========================================
            // SIGNS / BRACKETS
            // ==========================================
            else if (kind == TokenType::PARENTESIS_OPEN) {
                // Handled when token is '('
            } 
            else if (kind == TokenType::PARENTESIS_CLOSE) {
                // Handled when token is ')'
            } 
            else if (kind == TokenType::BRAKET_OPEN) {
                // Handled when token is '{'
            } 
            else if (kind == TokenType::BRAKET_CLOSE) {
                // Handled when token is '}'
            } 

            // ==========================================
            // END OF FILE / FAILLBACK
            // ==========================================
            else if (kind == TokenType::END_OF_FILE) {
                // Handled when you reach the end of the token stream
            } 
            else {
                // Fallback: This code should theoretically never be reached
            }
        }

        return nullptr; //CHANGE BEFORE COMPILING
    };

    private:
    std::vector<Token> tokens_to_parse;
    size_t index_token;
    size_t start;
    size_t end;

    size_t get_position() {

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

    std::string str = "int a = 10 int b = 2 print a + b"; 
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

