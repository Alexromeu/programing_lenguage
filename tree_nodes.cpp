#pragma once
#include <cstdlib>
#include <vector>
#include <unordered_map>
#include <string>

struct ASTNode;
struct Identifier;
struct Literal;
struct BlockStatement;

enum class Operator {
    Add, Sub, Mul, Div, Assign, Equal, NotEqual, LessThan, Unknown
};

// The Look-up Table
const std::unordered_map<std::string, Operator> StringToOperator = {
    {"+",  Operator::Add},
    {"-",  Operator::Sub},
    {"*",  Operator::Mul},
    {"/",  Operator::Div},
    {"=",  Operator::Assign},
    {"==", Operator::Equal},
    {"!=", Operator::NotEqual},
    {"<",  Operator::LessThan}
};

typedef struct FunctionDeclaration {
    const char* type;
    size_t start;
    size_t end;

    Identifier* id;
    bool expression;
    bool generator;
    bool async;
    std::vector<Identifier*> params;  

    BlockStatement* body;

} FunctionDeclaration;

typedef struct ReturnStatement {//goes within the body of a function
    const char* type;
    size_t start;
    size_t end;
    
    ASTNode* argument;
} ReturnStatement;

typedef struct AssignmentExpression {
    const char* type;
    size_t start;
    size_t end;

    Operator operator_;
    ASTNode* left;
    ASTNode* right;

} AssignmentExpression;

typedef struct Program {
    const char* type;
    size_t start;
    size_t end;
    std::vector<ASTNode*> body;

} Program;

typedef struct ExpressionStatement {
    const char* type;
    size_t start;
    size_t end;
    
    AssignmentExpression* expression;
    AssignmentExpression* alternate;
    
} ExpressionStatement;

typedef struct VariableDeclaration {
    const char* type;
    size_t start;
    size_t end;
    std::vector<ASTNode*> declarations;

} VariableDeclaration;

typedef struct VariableDeclarator {
    const char* type;
    size_t start;
    size_t end;

    Identifier *id;
    ASTNode *init;   

} VariableDeclarator;

typedef struct BlockStatement {
    const char* type;
    size_t start;
    size_t end;

    std::vector<ASTNode*> body;
} BlockStatement;

typedef struct BinaryExpression {
    const char* type;
    size_t start;
    size_t end;

    ASTNode* left;
    Operator operator_;
    ASTNode* right;
} BinaryExpression;

typedef struct IfStatement {
    const char* type;
    size_t start;
    size_t end;

    BinaryExpression* test;
    BlockStatement* consequent;
    BlockStatement* alternate;

} IfStatement;

typedef struct Literal {
    const char* type;
    size_t start;
    size_t end; 
    int value;
    char* raw; //raw = "value"

} Literal;

typedef struct Identifier {
    const char* type;
    size_t start;
    size_t end;
    char* name;
       
} Identifier;

typedef struct ASTNode {
    const char* type;
    size_t start;
    size_t end;

    union {
        Program* program;
        VariableDeclaration* variableDeclaration;
        VariableDeclarator* variableDeclarator;
        Identifier* identifier;
        Literal* literal;
        IfStatement* ifStatement;
        BinaryExpression* binaryExpression;
        FunctionDeclaration* functionDeclaration;
        AssignmentExpression* assignmentExpression;
        ExpressionStatement* expressionStatement;
        BlockStatement* blockStatement;

    } as;
} ASTNode;