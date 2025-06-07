#ifndef Token_hh
#define Token_hh

#include "./TokenType.hh"
#include <string>

struct Token {
    TokenType type;
    std::string value;
    int line;
    int column;
    
    inline Token(TokenType t, const std::string& v, int l, int c) 
        : type(t), value(v), line(l), column(c) {}
};

#endif