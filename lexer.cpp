#pragma once
#include <cctype>
#include <cstddef>
#include <cstdio>
#include <string>
#include <unordered_map>
#include <vector>



enum class TokenType {
    // literals
    NUMBER,        // 10, 42        -> a numeric literal
    UNIDENTIFIED,  // a, b, foo     -> an identifier (a word not in the keyword map)

    // data-type keywords
    INTEGER,       // int
    FLOAT,         // float
    BOOLEAN,       // bool
    CHAR,          // char

    // arithmetic / assignment operators
    PLUS,          // +
    MINUS,         // -
    STAR,          // *
    DIVIDE,        // /
    EQUALS,        // =

    // comparison operators (need maximal munch: = vs ==, < vs <=)
    EQ_EQ,         // ==
    BANG_EQ,       // !=
    LESS,          // <
    LESS_EQ,       // <=
    GREATER,       // >
    GREATER_EQ,    // >=

    // keywords
    PRINT,         // printOut
    FUNCTION,      // function
    IF,            // if
    ELSE,          // else
    WHILE,         // while
    FOR,           // for

    // punctuation
    PARENTESIS_OPEN,   // (
    PARENTESIS_CLOSE,  // )
    BRAKET_OPEN,       // {
    BRAKET_CLOSE,      // }
    COMMA,             // ,

    // structural
    END_OF_FILE,
    UNKNOWN        // a character the lexer did not recognise (error)
};

// Only *words* are looked up here (keywords + type names). Operators and
// punctuation are matched character-by-character inside the lexer instead,
// so multi-char operators like == work via maximal munch.
const std::unordered_map<std::string, TokenType> stringToTypeMap = {
    {"int",      TokenType::INTEGER},
    {"float",    TokenType::FLOAT},
    {"bool",     TokenType::BOOLEAN},
    {"char",     TokenType::CHAR},

    {"printOut", TokenType::PRINT},
    {"function", TokenType::FUNCTION},
    {"if",       TokenType::IF},
    {"else",     TokenType::ELSE},
    {"while",    TokenType::WHILE},
    {"for",      TokenType::FOR},
};

struct Token {
    TokenType   kind;    // the category (enum, not a string)
    std::string value;   // the raw source text: "10", "a", "while", "=="
    int         line;    // 1-based, where the token starts
    int         column;  // 1-based, where the token starts
};

// ============================================================
//  Lexer
//  Walks the source ONE character at a time with a cursor.
//  No whitespace-splitting: token boundaries come from character
//  categories (digit / letter / operator), not from spaces.
// ============================================================

class Lexer {
public:
    explicit Lexer(std::string source) : src(std::move(source)) {}

    std::vector<Token> tokenize() {
        std::vector<Token> output;
        
        while (!at_end()) {
            // remember where this token starts (for the stamp)
            tok_start_line = line;
            tok_start_col  = column;

            char c = peek();

            // ---- whitespace: skip, but count newlines ----
            if (c == ' ' || c == '\t' || c == '\r') { advance(); continue; }
            if (c == '\n') { advance(); line++; column = 1; continue; }

            // ---- number: a run of digits ----
            if (std::isdigit(static_cast<unsigned char>(c))) {
                std::size_t start = pos;

                while (std::isdigit(static_cast<unsigned char>(peek()))) advance();

                output.push_back(make(TokenType::NUMBER, src.substr(start, pos - start)));
                continue;
            }

            // ---- word: a run of letters/digits/underscore, then look it up ----
            if (std::isalpha(static_cast<unsigned char>(c)) || c == '_') {
                std::size_t start = pos;

                while (std::isalnum(static_cast<unsigned char>(peek())) || peek() == '_')
                    advance();

                std::string word = src.substr(start, pos - start);
                auto it = stringToTypeMap.find(word);
                TokenType kind = (it != stringToTypeMap.end())
                                     ? it->second              // keyword / type
                                     : TokenType::UNIDENTIFIED; // identifier
                output.push_back(make(kind, word));
                continue;
            }

            // ---- operators & punctuation (maximal munch) ----
            advance(); // consume c; we already captured the start position
            switch (c) {
                case '+': output.push_back(make(TokenType::PLUS,  "+")); break;
                case '-': output.push_back(make(TokenType::MINUS, "-")); break;
                case '*': output.push_back(make(TokenType::STAR,  "*")); break;
                case '(': output.push_back(make(TokenType::PARENTESIS_OPEN,  "(")); break;
                case ')': output.push_back(make(TokenType::PARENTESIS_CLOSE, ")")); break;
                case '{': output.push_back(make(TokenType::BRAKET_OPEN,  "{")); break;
                case '}': output.push_back(make(TokenType::BRAKET_CLOSE, "}")); break;
                case ',': output.push_back(make(TokenType::COMMA, ",")); break;

                // '/' could start a // comment
                case '/':
                    if (match('/')) {                       // line comment
                        while (!at_end() && peek() != '\n') advance();
                    } else {
                        output.push_back(make(TokenType::DIVIDE, "/"));
                    }
                    break;

                // the munch cases: one char or two?
                case '=':
                    if (match('=')) output.push_back(make(TokenType::EQ_EQ,  "=="));
                    else            output.push_back(make(TokenType::EQUALS, "="));
                    break;
                case '!':
                    if (match('=')) output.push_back(make(TokenType::BANG_EQ, "!="));
                    else            output.push_back(make(TokenType::UNKNOWN, "!")); // lone ! not supported yet
                    break;
                case '<':
                    if (match('=')) output.push_back(make(TokenType::LESS_EQ, "<="));
                    else            output.push_back(make(TokenType::LESS,    "<"));
                    break;
                case '>':
                    if (match('=')) output.push_back(make(TokenType::GREATER_EQ, ">="));
                    else            output.push_back(make(TokenType::GREATER,    ">"));
                    break;

                default:
                    // unknown character — emit an UNKNOWN token so the parser
                    // can report a clear error later instead of silently skipping
                    output.push_back(make(TokenType::UNKNOWN, std::string(1, c)));
                    break;
            }
        }

        output.push_back(make(TokenType::END_OF_FILE, ""));
        return output;
    }

    void printTokens(const std::vector<Token>& tokens) {
        for (const Token& tok : tokens) {
            printf("%-16s '%s'  (line %d, col %d)\n",
                   tokenTypeName(tok.kind), tok.value.c_str(), tok.line, tok.column);
        }
    }

private:
    std::string src;
    std::size_t pos    = 0;   // cursor: index into src, only moves forward
    int         line   = 1;
    int         column = 1;
    int         tok_start_line = 1;  // position where the current token began
    int         tok_start_col  = 1;

    bool at_end() const { return pos >= src.size(); }

    // look at the current char without consuming it
    char peek() const { return at_end() ? '\0' : src[pos]; }

    // consume the current char and move the cursor forward
    char advance() {
        char c = src[pos++];
        column++;
        return c;
    }

    // consume the current char ONLY if it equals `expected` (maximal munch)
    bool match(char expected) {
        if (at_end() || src[pos] != expected) return false;
        pos++;
        column++;
        return true;
    }

    // build a token stamped with where it started
    Token make(TokenType kind, std::string value) {
        return Token{ kind, std::move(value), tok_start_line, tok_start_col };
    }

    static const char* tokenTypeName(TokenType t) {
        switch (t) {
            case TokenType::NUMBER:           return "NUMBER";
            case TokenType::UNIDENTIFIED:     return "IDENTIFIER";
            case TokenType::INTEGER:          return "INT";
            case TokenType::FLOAT:            return "FLOAT";
            case TokenType::BOOLEAN:          return "BOOL";
            case TokenType::CHAR:             return "CHAR";
            case TokenType::PLUS:             return "PLUS";
            case TokenType::MINUS:            return "MINUS";
            case TokenType::STAR:             return "STAR";
            case TokenType::DIVIDE:           return "DIVIDE";
            case TokenType::EQUALS:           return "EQUALS";
            case TokenType::EQ_EQ:            return "EQ_EQ";
            case TokenType::BANG_EQ:          return "BANG_EQ";
            case TokenType::LESS:             return "LESS";
            case TokenType::LESS_EQ:          return "LESS_EQ";
            case TokenType::GREATER:          return "GREATER";
            case TokenType::GREATER_EQ:       return "GREATER_EQ";
            case TokenType::PRINT:            return "PRINT";
            case TokenType::FUNCTION:         return "FUNCTION";
            case TokenType::IF:               return "IF";
            case TokenType::ELSE:             return "ELSE";
            case TokenType::WHILE:            return "WHILE";
            case TokenType::FOR:              return "FOR";
            case TokenType::PARENTESIS_OPEN:  return "LPAREN";
            case TokenType::PARENTESIS_CLOSE: return "RPAREN";
            case TokenType::BRAKET_OPEN:      return "LBRACE";
            case TokenType::BRAKET_CLOSE:     return "RBRACE";
            case TokenType::COMMA:            return "COMMA";
            case TokenType::END_OF_FILE:      return "EOF";
            case TokenType::UNKNOWN:          return "UNKNOWN";
        }
        return "?";
    }
};
