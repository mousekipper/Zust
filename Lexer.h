#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <map>
#include <vector>
#include "AST/AST.h"

// ===== 렉서 (Lexer) =====
class Lexer {
private:
    std::string source;
    size_t pos;
    int line;
    int column;
    std::map<std::string, TokenType> keywords;
    
    void initKeywords() {
        keywords = {
            {"let", TokenType::LET}, {"fn", TokenType::FN},
            {"if", TokenType::IF}, {"while", TokenType::WHILE},
            {"for", TokenType::FOR}, {"foreach", TokenType::FOREACH},
            {"switch", TokenType::SWITCH}, {"case", TokenType::CASE},
            {"default", TokenType::DEFAULT}, {"namespace", TokenType::NAMESPACE},
            {"import", TokenType::IMPORT}, {"return", TokenType::RETURN},
            {"break", TokenType::BREAK}, {"continue", TokenType::CONTINUE},
            {"int", TokenType::INT}, {"float", TokenType::FLOAT},
            {"char", TokenType::CHAR}, {"byte", TokenType::BYTE},
            {"long", TokenType::LONG}, {"double", TokenType::DOUBLE},
            {"short", TokenType::SHORT}, {"bool", TokenType::BOOL},
            {"string", TokenType::STRING}, {"void", TokenType::VOID},
            {"true", TokenType::BOOL_LITERAL}, {"false", TokenType::BOOL_LITERAL}
        };
    }
    
    char peek(int offset = 0) {
        if (pos + offset >= source.length()) return '\0';
        return source[pos + offset];
    }
    
    char advance() {
        if (pos >= source.length()) return '\0';
        char c = source[pos++];
        if (c == '\n') {
            line++;
            column = 1;
        } else {
            column++;
        }
        return c;
    }
    
    void skipWhitespace() {
        while (pos < source.length() && isspace(peek()) && peek() != '\n') {
            advance();
        }
    }
    
    Token readNumber() {
        std::string num;
        int startLine = line, startCol = column;
        bool isFloat = false;
        
        while (pos < source.length() && (isdigit(peek()) || peek() == '.')) {
            if (peek() == '.') {
                if (isFloat) break; // 두 번째 점은 허용하지 않음
                isFloat = true;
            }
            num += advance();
        }
        
        TokenType type = isFloat ? TokenType::FLOAT_LITERAL : TokenType::INTEGER_LITERAL;
        return Token(type, num, startLine, startCol);
    }
    
    Token readString() {
        std::string str;
        int startLine = line, startCol = column;
        advance(); // 시작 따옴표 건너뛰기
        
        while (pos < source.length() && peek() != '"') {
            if (peek() == '\\') {
                advance(); // 백슬래시 건너뛰기
                char escaped = advance();
                switch (escaped) {
                    case 'n': str += '\n'; break;
                    case 't': str += '\t'; break;
                    case 'r': str += '\r'; break;
                    case '\\': str += '\\'; break;
                    case '"': str += '"'; break;
                    default: str += escaped; break;
                }
            } else {
                str += advance();
            }
        }
        
        if (peek() == '"') advance(); // 끝 따옴표 건너뛰기
        return Token(TokenType::STRING_LITERAL, str, startLine, startCol);
    }
    
    Token readChar() {
        int startLine = line, startCol = column;
        advance(); // 시작 단일 따옴표 건너뛰기
        
        char c = advance();
        if (peek() == '\'') advance(); // 끝 단일 따옴표 건너뛰기
        
        return Token(TokenType::CHAR_LITERAL, std::string(1, c), startLine, startCol);
    }
    
    Token readIdentifier() {
        std::string id;
        int startLine = line, startCol = column;
        
        while (pos < source.length() && (isalnum(peek()) || peek() == '_')) {
            id += advance();
        }
        
        TokenType type = TokenType::IDENTIFIER;
        if (keywords.find(id) != keywords.end()) {
            type = keywords[id];
        }
        
        return Token(type, id, startLine, startCol);
    }
    
    Token readComment() {
        std::string comment;
        int startLine = line, startCol = column;
        advance(); // # 건너뛰기
        
        while (pos < source.length() && peek() != '\n') {
            comment += advance();
        }
        
        return Token(TokenType::COMMENT, comment, startLine, startCol);
    }
    
public:
    Lexer(const std::string& src) : source(src), pos(0), line(1), column(1) {
        initKeywords();
    }
    
    std::vector<Token> tokenize() {
        std::vector<Token> tokens;
        
        while (pos < source.length()) {
            skipWhitespace();
            
            if (pos >= source.length()) break;
            
            char c = peek();
            int startLine = line, startCol = column;
            
            if (c == '\n') {
                advance();
                tokens.emplace_back(TokenType::NEWLINE, "\\n", startLine, startCol);
            }
            else if (c == '#') {
                tokens.push_back(readComment());
            }
            else if (isdigit(c)) {
                tokens.push_back(readNumber());
            }
            else if (c == '"') {
                tokens.push_back(readString());
            }
            else if (c == '\'') {
                tokens.push_back(readChar());
            }
            else if (isalpha(c) || c == '_') {
                tokens.push_back(readIdentifier());
            }
            else {
                // 연산자 및 구분자 처리
                switch (c) {
                    case '+': advance(); tokens.emplace_back(TokenType::PLUS, "+", startLine, startCol); break;
                    case '-': advance(); tokens.emplace_back(TokenType::MINUS, "-", startLine, startCol); break;
                    case '*': advance(); tokens.emplace_back(TokenType::MULTIPLY, "*", startLine, startCol); break;
                    case '/': advance(); tokens.emplace_back(TokenType::DIVIDE, "/", startLine, startCol); break;
                    case '%': advance(); tokens.emplace_back(TokenType::MODULO, "%", startLine, startCol); break;
                    case '=':
                        advance();
                        if (peek() == '=') {
                            advance();
                            tokens.emplace_back(TokenType::EQUAL, "==", startLine, startCol);
                        } else {
                            tokens.emplace_back(TokenType::ASSIGN, "=", startLine, startCol);
                        }
                        break;
                    case '!':
                        advance();
                        if (peek() == '=') {
                            advance();
                            tokens.emplace_back(TokenType::NOT_EQUAL, "!=", startLine, startCol);
                        } else {
                            tokens.emplace_back(TokenType::LOGICAL_NOT, "!", startLine, startCol);
                        }
                        break;
                    case '<':
                        advance();
                        if (peek() == '=') {
                            advance();
                            tokens.emplace_back(TokenType::LESS_EQUAL, "<=", startLine, startCol);
                        } else if (peek() == '<') {
                            advance();
                            tokens.emplace_back(TokenType::LEFT_SHIFT, "<<", startLine, startCol);
                        } else {
                            tokens.emplace_back(TokenType::LESS, "<", startLine, startCol);
                        }
                        break;
                    case '>':
                        advance();
                        if (peek() == '=') {
                            advance();
                            tokens.emplace_back(TokenType::GREATER_EQUAL, ">=", startLine, startCol);
                        } else if (peek() == '>') {
                            advance();
                            tokens.emplace_back(TokenType::RIGHT_SHIFT, ">>", startLine, startCol);
                        } else {
                            tokens.emplace_back(TokenType::GREATER, ">", startLine, startCol);
                        }
                        break;
                    case '&':
                        advance();
                        if (peek() == '&') {
                            advance();
                            tokens.emplace_back(TokenType::LOGICAL_AND, "&&", startLine, startCol);
                        } else {
                            tokens.emplace_back(TokenType::BIT_AND, "&", startLine, startCol);
                        }
                        break;
                    case '|':
                        advance();
                        if (peek() == '|') {
                            advance();
                            tokens.emplace_back(TokenType::LOGICAL_OR, "||", startLine, startCol);
                        } else {
                            tokens.emplace_back(TokenType::BIT_OR, "|", startLine, startCol);
                        }
                        break;
                    case '^': advance(); tokens.emplace_back(TokenType::BIT_XOR, "^", startLine, startCol); break;
                    case '~': advance(); tokens.emplace_back(TokenType::BIT_NOT, "~", startLine, startCol); break;
                    case ';': advance(); tokens.emplace_back(TokenType::SEMICOLON, ";", startLine, startCol); break;
                    case ',': advance(); tokens.emplace_back(TokenType::COMMA, ",", startLine, startCol); break;
                    case '.': advance(); tokens.emplace_back(TokenType::DOT, ".", startLine, startCol); break;
                    case '(': advance(); tokens.emplace_back(TokenType::LPAREN, "(", startLine, startCol); break;
                    case ')': advance(); tokens.emplace_back(TokenType::RPAREN, ")", startLine, startCol); break;
                    case '{': advance(); tokens.emplace_back(TokenType::LBRACE, "{", startLine, startCol); break;
                    case '}': advance(); tokens.emplace_back(TokenType::RBRACE, "}", startLine, startCol); break;
                    case '[': advance(); tokens.emplace_back(TokenType::LBRACKET, "[", startLine, startCol); break;
                    case ']': advance(); tokens.emplace_back(TokenType::RBRACKET, "]", startLine, startCol); break;
                    default:
                        advance();
                        tokens.emplace_back(TokenType::UNKNOWN, std::string(1, c), startLine, startCol);
                        break;
                }
            }
        }
        
        tokens.emplace_back(TokenType::EOF_TOKEN, "", line, column);
        return tokens;
    }
};
#endif // LEXER_H