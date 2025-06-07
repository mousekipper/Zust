#include <Lexer.hh>

#undef ccfunc
#define ccfunc Lexer::

void ccfunc initKeywords() {
    this->keywords = {
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

char ccfunc peek(int offset) const {
    if (pos + offset >= source.length()) return '\0';
    return source[pos + offset];
}

void ccfunc skipWhitespace() {
    while (pos < source.length() && isspace(peek()) && peek() != '\n') {
        advance();
    }
}

char ccfunc advance() {
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

Token ccfunc readNumber() {
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


Token ccfunc readString() {
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

Token ccfunc readChar() {
    int startLine = line, startCol = column;
    advance(); // 시작 단일 따옴표 건너뛰기
    
    char c = advance();
    if (peek() == '\'') advance(); // 끝 단일 따옴표 건너뛰기
    
    return Token(TokenType::CHAR_LITERAL, std::string(1, c), startLine, startCol);
}

Token ccfunc readIdentifier() {
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

Token ccfunc readComment() {
    std::string comment;
    int startLine = line, startCol = column;
    advance(); // # 건너뛰기
    
    while (pos < source.length() && peek() != '\n') {
        comment += advance();
    }
    
    return Token(TokenType::COMMENT, comment, startLine, startCol);
}

std::vector<Token> ccfunc tokenize() {
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
                #define tokemplace(c, toktype)  tokens.emplace_back(toktype, c, startLine, startCol);
                #define caseone(c, toktype)     case (c)[0]: advance(); tokemplace(c, toktype); break;

                caseone("+", TokenType::PLUS);
                caseone("-", TokenType::MINUS);
                caseone("*", TokenType::MULTIPLY);
                caseone("/", TokenType::DIVIDE);
                caseone("%", TokenType::MODULO);
                caseone(":", TokenType::COLUMN);

                case '=':
                    advance();
                    if (peek() == '=') {
                        advance();
                        tokemplace("==", TokenType::EQUAL);
                    } else {
                        tokemplace("=", TokenType::ASSIGN);
                    }
                    break;
                case '!':
                    advance();
                    if (peek() == '=') {
                        advance();
                        tokemplace("!=", TokenType::NOT_EQUAL);
                    } else {
                        tokemplace("!", TokenType::LOGICAL_NOT);
                    }
                    break;
                case '<':
                    advance();

                    if (peek() == '=') {
                        advance();
                        tokemplace("<=", TokenType::LESS_EQUAL);
                    } else if (peek() == '<') {
                        advance();
                        tokemplace("<<", TokenType::LEFT_SHIFT);
                    } else {
                        tokemplace("<", TokenType::LESS);
                    }
                    break;
                case '>':
                    advance();
                    if (peek() == '=') {
                        advance();
                        tokemplace(">=", TokenType::GREATER_EQUAL);
                    } else if (peek() == '>') {
                        advance();
                        tokemplace(">>", TokenType::RIGHT_SHIFT);
                    } else {
                        tokemplace(">", TokenType::GREATER);
                    }
                    break;
                case '&':
                    advance();
                    if (peek() == '&') {
                        advance();
                        tokemplace("&&", TokenType::LOGICAL_AND);
                    } else {
                        tokemplace("&", TokenType::BIT_AND);
                    }
                    break;
                case '|':
                    advance();
                    if (peek() == '|') {
                        advance();
                        tokemplace("||", TokenType::LOGICAL_OR);
                    } else {
                        tokemplace("|", TokenType::BIT_OR);
                    }
                    break;

                caseone("^", TokenType::BIT_XOR);
                caseone("~", TokenType::BIT_NOT);
                caseone(";", TokenType::SEMICOLON);
                caseone(",", TokenType::COMMA);
                caseone(".", TokenType::DOT);
                caseone("(", TokenType::LPAREN);
                caseone(")", TokenType::RPAREN);
                caseone("{", TokenType::LBRACE);
                caseone("}", TokenType::RBRACE);
                caseone("[", TokenType::LBRACKET);
                caseone("]", TokenType::RBRACKET);
                default:
                    advance();
                    tokens.emplace_back(TokenType::UNKNOWN, std::string(1, c), startLine, startCol);
                    break;

                #undef caseone
                #undef tokemplace
            }
        }
    }
    
    tokens.emplace_back(TokenType::EOF_TOKEN, "", line, column);
    return tokens;
}