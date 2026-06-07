#include <cstddef>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>


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

    //internal
    PRINT,
    UNIDENTIFIED,
    FUNCTION,

    //control flow
    IF,
    ELSE,
    WHILE,
    FOR,
    
    //signs
    PARENTESIS_OPEN,
    PARENTESIS_CLOSE,
    BRAKET_OPEN,
    BRAKET_CLOSE,    

    // Structural / Invisible Tokens 
    END_OF_FILE
};

const std::unordered_map<std::string, TokenType> stringToTypeMap = {
    // Data Type Identifiers / Keywords
    {"int",         TokenType::INTEGER},
    {"float",       TokenType::FLOAT,},
    {"bool",        TokenType::BOOLEAN},
    {"char",        TokenType::CHAR},
    
    // Core Keywords / Special System Names
    {"printOut",       TokenType::PRINT},
    {"function",       TokenType::FUNCTION},

    // Operators
    {"+",           TokenType::PLUS},
    {"-",           TokenType::MINUS},
    {"*",           TokenType::STAR},
    {"/",          TokenType::DIVIDE},
    {"=",          TokenType::EQUALS},

    //signs
    {"(",          TokenType::PARENTESIS_OPEN },
    {")",          TokenType::PARENTESIS_CLOSE },
    {"{",          TokenType::BRAKET_OPEN },
    {"}",          TokenType::BRAKET_CLOSE },

    //control flow
    {"if",             TokenType::IF},
    {"else",           TokenType::ELSE},
    {"while",          TokenType::WHILE},
    {"for",            TokenType::FOR},

    // Other
    {"EOF",        TokenType::END_OF_FILE},
    {"unidentified",   TokenType::UNIDENTIFIED},
};


//-----token types
namespace TokenNewLine {
    std::string NEWLINE = "_nln_"; //new line
};


struct Token {
    TokenType kind; //NUMBER, IDENT, etc.
    std::string value; //"12", "x", "while", "if", etc.
};
//------




struct ExpressionBinary {
    char op;           // '+', '-', '*', '/'
    struct ASTNode* left;
    struct ASTNode* right;
};


struct Block {
    struct ASTNode** statements;  // array of child nodes
    size_t count;
};


struct VarType {
    union {
        int integer;
        char character;
        std::string string;
        bool boolean;   
    };
};


struct DataStructure {
    union {
        std::vector<VarType> array;
        std::map<VarType, VarType> map;
        std::unordered_map<VarType, VarType> unordered_map;
    };
};

//-----Functions
struct Parameter {
    VarType param;
};


struct ParameterList {
    std::vector<Parameter*> parameters;
};



//-------

struct NumberLiteral {
    union {
        int int_value;
        float float_value;
    } ;
};



//after tokens are made we need to create the expressions at runtime with switch

