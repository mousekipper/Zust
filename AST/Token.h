#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include "TokenType.h"

struct Token {
    TokenType type;
    std::string value;
    int line;
    int column;
    
    Token(TokenType t, const std::string& v, int l, int c) 
        : type(t), value(v), line(l), column(c) {}
};

#endif // TOKEN_H