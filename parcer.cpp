#include <cstddef>
#include <string>
#include <vector>
#include <string>
#include <string.h>
#include <unordered_map>
#include <typeinfo>



enum class TokenType {
    // data types
    INTEGER,
    FLOAT,
    BOOLEAN,
    CHAR, 
    
    // Operators
    PLUS,
    MINUS,
    STAR,
    EQUALS,
    DIVIDE,

    PRINT,
    UNIDENTIFIED,
    // Structural / Invisible Tokens 
    END_OF_FILE  // Tells your parser when the text is completely finished
};

namespace TokenString {
    std::string NEWLINE = "_newline_";
}

struct Token {
    TokenType kind; //NUMBER, IDENT, etc.
    std::string value; //"12", "x", "while", "if", etc.
};

struct Expression {
    std::string atomic;
    std::pair<std::string, std::vector<std::string>> operation;

    // Expression exp
    
    // exp.operation({'-', {a, b}})
    // a + b * c
    // exp.operation({'+', 
    //          exp.operation({ '*',
    //                  {b, c} })})
};


const std::unordered_map<std::string, TokenType> stringToTypeMap = {
    // Data Type Identifiers / Keywords
    {"int",         TokenType::INTEGER},
    {"float",       TokenType::FLOAT,},
    {"bool",        TokenType::BOOLEAN},
    {"char",        TokenType::CHAR},
    
    // Core Keywords / Special System Names
    {"print",       TokenType::PRINT},
    

    // Operators
    {"+",           TokenType::PLUS},
    {"-",           TokenType::MINUS},
    {"*",           TokenType::STAR},
    {"/",           TokenType::DIVIDE},
    {"=",          TokenType::EQUALS},

    // Other
    {"EOF",        TokenType::END_OF_FILE},
    {"unidentified",   TokenType::UNIDENTIFIED},
};




class Scanner {
    public:
    Scanner() {};

    std::vector<std::string> split_text(std::string &str_in) {
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
                TextChunksList.push_back(TokenString::NEWLINE);
                next_line_pos = str_in.find("\n", next_line_pos + 1);
            }
            TextChunksList.push_back(extracted_text);

            extracted_text = strtok(NULL, " \t\r\n");
        } 

        while (next_line_pos != std::string::npos) {
            TextChunksList.push_back(TokenString::NEWLINE);
        }

        return TextChunksList;
    }

    void printTokenList(std::vector<std::string> &TextChunksList) {
        size_t list_size = TextChunksList.size();
        for (size_t i = 0; i < list_size; i++) {
            std::string s = TextChunksList.at(i);
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
    public:
    Parser() {};
    
};

class AST {
    public:
};



int main() {
    Scanner sc;
    Lexer lex;
    std::string str = "int a = 10 int b = 2 print a + b"; 
    std::vector<std::string> textList = sc.split_text(str);

    std::vector<Token> tokenS = lex.tokenize(textList);
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
