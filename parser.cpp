#include <cstddef>
#include <vector>
#include <string.h>
#include <string>
#include "structs.cpp"


class Scanner {
    public:
    Scanner() {};

    std::vector<std::string>* split_text(std::string &str_in) {
        std::vector<char> buffer(str_in.begin(), str_in.end());
        std::vector<std::string> TextChunksList; 
        buffer.push_back('\0');
        char* raw_text = buffer.data();
        size_t next_line_pos = str_in.find("\n", 0);
        char* extracted_text = strtok(raw_text, "\t\r\n ");

        size_t current_search_offset = 0;

        while (extracted_text != NULL) {
            size_t token_posInSource = str_in.find(extracted_text, current_search_offset);
            
            while (next_line_pos != std::string::npos && next_line_pos < token_posInSource) {
                TextChunksList.push_back(TokenNewLine::NEWLINE);
                next_line_pos = str_in.find("\n", next_line_pos + 1);
            }
            TextChunksList.push_back(extracted_text);

            extracted_text = strtok(NULL, " \t\r\n");
        } 

        while (next_line_pos != std::string::npos) {
            TextChunksList.push_back(TokenNewLine::NEWLINE);
        }

        std::vector<std::string>* textPointer = &TextChunksList;

        return textPointer;
    }

    void printTokenList(std::vector<std::string>* TextChunksList) {
        
        size_t list_size = (*TextChunksList).size();
        for (size_t i = 0; i < list_size; i++) {
            std::string s = (*TextChunksList).at(i);
            char* conv_str = const_cast<char*>(s.data());
            printf("tok-> %s\n", conv_str);
        }
    }
};

class Lexer {
    public:
    Lexer() {};

    std::vector<Token> tokenize(std::vector<std::string> &chunkList) {
        std::vector<Token> output = {};
        for (size_t i = 0; i < chunkList.size(); i++) {
            const std::string curr_tok = chunkList[i];
            auto it = stringToTypeMap.find(curr_tok);
            if (it != stringToTypeMap.end()) {
                Token token1 = {};
                token1.kind = it->second;
                token1.value = it->first;
                output.push_back(token1);

            } else {
                Token token2 = {};
                token2.kind = TokenType::UNIDENTIFIED; //later if a var is unidentify we need to search back a few to look to the type
                token2.value = curr_tok;
                output.push_back(token2);
            }  
        }

        return output;
    };

    void printTokens(std::vector<Token> &tokenList) {
        for (Token tok : tokenList) {
            printf("%d -> %s\n", static_cast<int>(tok.kind), (tok.value).data());
        }
    }
};

class Parser {
    //identifies the Tokens and creates structures 
    public:
    Parser() { index_token = 0; };
    volatile size_t index_token;
    

    ASTNode* parse_token(Token &token) {
        ASTNode *out_node = nullptr;

        while (peek().kind != TokenType::END_OF_FILE) {
            Token token = advance();
            TokenType kind = token.kind;
            std::string value = token.value;

            
            // ==========================================
            // DATA TYPES
            // ==========================================
            if (kind == TokenType::INTEGER) {
                Token varName = advance();
                Token equals = advance();
                Token amount = advance();

                Identifier identifier1;
                identifier1.name = varName.value.data();

                out_node->type = kind;
                out_node->as.var_decl.name = identifier1;

                return out_node;
            } 
            else if (kind == TokenType::FLOAT) {
                // Handled when token is a 'float' keyword
            } 
            else if (kind == TokenType::BOOLEAN) {
                // Handled when token is a 'bool' keyword
            } 
            else if (kind == TokenType::CHAR) {
                // Handled when token is a 'char' keyword
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
    };

    private:
    std::vector<Token> tokens_to_parse;

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
    Scanner sc;
    Lexer lex;
    std::string str = "int a = 10 int b = 2 print a + b"; 
    std::vector<std::string> *textList = sc.split_text(str);

    std::vector<Token> tokenS = lex.tokenize(*textList);
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

