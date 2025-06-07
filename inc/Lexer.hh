#ifndef Lexer_hh
#define Lexer_hh

#include "./Token.hh" 
#include <map>
#include <vector>

#define ccfunc

// ===== 렉서 (Lexer) =====
class Lexer {
private:
    std::string source;
    size_t pos;
    int line;
    int column;
    std::map<std::string, TokenType> keywords;
    
    void ccfunc initKeywords();
    char ccfunc peek(int offset = 0) const;
    char ccfunc advance();
    void ccfunc skipWhitespace();
    
    Token ccfunc readNumber();
    Token ccfunc readString();
    Token ccfunc readChar();
    Token ccfunc readIdentifier();
    Token ccfunc readComment();

    
public:
    inline Lexer(const std::string& src) : source(src), pos(0), line(1), column(1) {
        initKeywords();
    }
    
    std::vector<Token> ccfunc tokenize();
    
};


#endif