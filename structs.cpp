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

namespace TokenNewLine {
    std::string NEWLINE = "_nln_"; //new line
};


struct Token {
    TokenType kind; //NUMBER, IDENT, etc.
    std::string value; //"12", "x", "while", "if", etc.
};


struct Identifier {
    char* name;   // just the variable name as a string
};

struct ExpressionVarDecl {
    Identifier name;
    struct ASTNode* value;  // points to whatever expression
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

struct Parameter {
    VarType param;
};

struct ParameterList {
    std::vector<Parameter*> parameters;
};


struct Function {
    struct Identifier name;       // The name used to call it
    struct ParameterList* params;  // The inputs (names and types)
    struct Block* body;            // The actual block of code to execute
    struct VarType returnType;       // The output type (or void)
};


struct ExpressionBinary {
    char op;           // '+', '-', '*', '/'
    struct ASTNode* left;
    struct ASTNode* right;
};


struct NumberLiteral {
    union {
        int int_value;
        float float_value;
    };
};


typedef struct ASTNode {
    TokenType type;     
    union {
        ExpressionVarDecl     var_decl;
        ExpressionBinary  binary_expr;
        NumberLiteral number;
        Identifier  identifier;
        Block*       block;
    } as;              
} ASTNode;


