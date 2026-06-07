#include <cstdlib>
#include <vector>

struct ASTNode;
struct Identifier;
struct Literal;

typedef struct Program {
    char* type;
    size_t start;
    size_t end;
    std::vector<ASTNode*> body;

} Program;

typedef struct VariableDeclaration {
    char* type;
    size_t start;
    size_t end;
    std::vector<ASTNode*> declarations;

} VariableDeclaration;

typedef struct VariableDeclarator {
    char* type;
    size_t start;
    size_t end;

    Identifier *id;
    Literal *init;   

} VariableDeclarator;

typedef struct Literal {
    char* type;
    size_t start;
    size_t end; 
    int value;
    char* raw; //raw = "value"

} Literal;

typedef struct Identifier {
    char* type;
    size_t start;
    size_t end;
    char* name;
       
} Identifier;

typedef struct ASTNode {
    char* type;
    size_t start;
    size_t end;

    union {
        Program* Program;
        VariableDeclaration* VariableDeclaration;
        VariableDeclarator* VariableDeclarator;
        Identifier* Identifier;
        Literal* Literal;
    } as;
} ASTNode;