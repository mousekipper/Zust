#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <memory>
#include <fstream>
#include <sstream>
#include <cctype>
#include <stdexcept>
#include <variant>
#include <stack>

// ===== 토큰 정의 =====
enum class TokenType {
    // 키워드
    LET, FN, IF, WHILE, FOR, FOREACH, SWITCH, CASE, DEFAULT,
    NAMESPACE, IMPORT, RETURN, BREAK, CONTINUE,
    
    // 자료형
    INT, FLOAT, CHAR, BYTE, LONG, DOUBLE, SHORT, BOOL,
    STRING, VOID,
    
    // 연산자
    PLUS, MINUS, MULTIPLY, DIVIDE, MODULO,
    ASSIGN, EQUAL, NOT_EQUAL, LESS, GREATER, LESS_EQUAL, GREATER_EQUAL,
    LOGICAL_AND, LOGICAL_OR, LOGICAL_NOT,
    BIT_AND, BIT_OR, BIT_XOR, BIT_NOT, LEFT_SHIFT, RIGHT_SHIFT,
    
    // 구분자
    SEMICOLON, COMMA, DOT,
    LPAREN, RPAREN, LBRACE, RBRACE, LBRACKET, RBRACKET,
    
    // 리터럴
    INTEGER_LITERAL, FLOAT_LITERAL, CHAR_LITERAL, STRING_LITERAL,
    BOOL_LITERAL,
    
    // 식별자
    IDENTIFIER,
    
    // 기타
    COMMENT, NEWLINE, EOF_TOKEN, UNKNOWN
};

struct Token {
    TokenType type;
    std::string value;
    int line;
    int column;
    
    Token(TokenType t, const std::string& v, int l, int c) 
        : type(t), value(v), line(l), column(c) {}
};

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

// ===== AST 노드 정의 =====
enum class NodeType {
    PROGRAM, VARIABLE_DECLARATION, FUNCTION_DECLARATION,
    BLOCK_STATEMENT, IF_STATEMENT, WHILE_STATEMENT, FOR_STATEMENT,
    RETURN_STATEMENT, EXPRESSION_STATEMENT,
    BINARY_EXPRESSION, UNARY_EXPRESSION, CALL_EXPRESSION,
    IDENTIFIER, INTEGER_LITERAL, FLOAT_LITERAL, STRING_LITERAL,
    CHAR_LITERAL, BOOL_LITERAL, ASSIGNMENT_EXPRESSION,
    NAMESPACE_DECLARATION, IMPORT_STATEMENT
};

struct ASTNode {
    NodeType type;
    virtual ~ASTNode() = default;
};

struct Program : ASTNode {
    std::vector<std::unique_ptr<ASTNode>> statements;
    Program() { type = NodeType::PROGRAM; }
};

struct VariableDeclaration : ASTNode {
    std::string dataType;
    std::string name;
    std::unique_ptr<ASTNode> initializer;
    
    VariableDeclaration() { type = NodeType::VARIABLE_DECLARATION; }
};

struct FunctionDeclaration : ASTNode {
    std::string returnType;
    std::string name;
    std::vector<std::pair<std::string, std::string>> parameters; // type, name
    std::unique_ptr<ASTNode> body;
    
    FunctionDeclaration() { type = NodeType::FUNCTION_DECLARATION; }
};

struct BlockStatement : ASTNode {
    std::vector<std::unique_ptr<ASTNode>> statements;
    BlockStatement() { type = NodeType::BLOCK_STATEMENT; }
};

struct IfStatement : ASTNode {
    std::unique_ptr<ASTNode> condition;
    std::unique_ptr<ASTNode> thenStatement;
    std::unique_ptr<ASTNode> elseStatement;
    
    IfStatement() { type = NodeType::IF_STATEMENT; }
};

struct WhileStatement : ASTNode {
    std::unique_ptr<ASTNode> condition;
    std::unique_ptr<ASTNode> body;
    
    WhileStatement() { type = NodeType::WHILE_STATEMENT; }
};

struct ReturnStatement : ASTNode {
    std::unique_ptr<ASTNode> expression;
    ReturnStatement() { type = NodeType::RETURN_STATEMENT; }
};

struct ExpressionStatement : ASTNode {
    std::unique_ptr<ASTNode> expression;
    ExpressionStatement() { type = NodeType::EXPRESSION_STATEMENT; }
};

struct BinaryExpression : ASTNode {
    std::unique_ptr<ASTNode> left;
    TokenType operator_;
    std::unique_ptr<ASTNode> right;
    
    BinaryExpression() { type = NodeType::BINARY_EXPRESSION; }
};

struct UnaryExpression : ASTNode {
    TokenType operator_;
    std::unique_ptr<ASTNode> operand;
    
    UnaryExpression() { type = NodeType::UNARY_EXPRESSION; }
};

struct CallExpression : ASTNode {
    std::unique_ptr<ASTNode> callee;
    std::vector<std::unique_ptr<ASTNode>> arguments;
    
    CallExpression() { type = NodeType::CALL_EXPRESSION; }
};

struct Identifier : ASTNode {
    std::string name;
    Identifier(const std::string& n) : name(n) { type = NodeType::IDENTIFIER; }
};

struct IntegerLiteral : ASTNode {
    int value;
    IntegerLiteral(int v) : value(v) { type = NodeType::INTEGER_LITERAL; }
};

struct FloatLiteral : ASTNode {
    double value;
    FloatLiteral(double v) : value(v) { type = NodeType::FLOAT_LITERAL; }
};

struct StringLiteral : ASTNode {
    std::string value;
    StringLiteral(const std::string& v) : value(v) { type = NodeType::STRING_LITERAL; }
};

struct BoolLiteral : ASTNode {
    bool value;
    BoolLiteral(bool v) : value(v) { type = NodeType::BOOL_LITERAL; }
};

struct AssignmentExpression : ASTNode {
    std::unique_ptr<ASTNode> left;
    std::unique_ptr<ASTNode> right;
    
    AssignmentExpression() { type = NodeType::ASSIGNMENT_EXPRESSION; }
};

struct NamespaceDeclaration : ASTNode {
    std::string name;
    std::unique_ptr<ASTNode> body;
    
    NamespaceDeclaration() { type = NodeType::NAMESPACE_DECLARATION; }
};

// ===== 파서 (Parser) =====
class Parser {
private:
    std::vector<Token> tokens;
    size_t pos;
    
    Token& current() {
        if (pos >= tokens.size()) return tokens.back(); // EOF 토큰
        return tokens[pos];
    }
    
    Token& peek(int offset = 1) {
        if (pos + offset >= tokens.size()) return tokens.back();
        return tokens[pos + offset];
    }
    
    bool match(TokenType type) {
        if (current().type == type) {
            pos++;
            return true;
        }
        return false;
    }
    
    void expect(TokenType type) {
        if (!match(type)) {
            throw std::runtime_error("Expected token type at line " + 
                std::to_string(current().line));
        }
    }
    
    void skipNewlines() {
        while (match(TokenType::NEWLINE) || match(TokenType::COMMENT)) {}
    }
    
    std::unique_ptr<ASTNode> parseExpression();
    std::unique_ptr<ASTNode> parseAssignmentExpression();
    std::unique_ptr<ASTNode> parseLogicalOrExpression();
    std::unique_ptr<ASTNode> parseLogicalAndExpression();
    std::unique_ptr<ASTNode> parseBitwiseOrExpression();
    std::unique_ptr<ASTNode> parseBitwiseXorExpression();
    std::unique_ptr<ASTNode> parseBitwiseAndExpression();
    std::unique_ptr<ASTNode> parseEqualityExpression();
    std::unique_ptr<ASTNode> parseRelationalExpression();
    std::unique_ptr<ASTNode> parseShiftExpression();
    std::unique_ptr<ASTNode> parseAdditiveExpression();
    std::unique_ptr<ASTNode> parseMultiplicativeExpression();
    std::unique_ptr<ASTNode> parseUnaryExpression();
    std::unique_ptr<ASTNode> parsePostfixExpression();
    std::unique_ptr<ASTNode> parsePrimaryExpression();
    
public:
    Parser(const std::vector<Token>& toks) : tokens(toks), pos(0) {}
    
    std::unique_ptr<Program> parse() {
        auto program = std::make_unique<Program>();
        
        skipNewlines();
        
        while (current().type != TokenType::EOF_TOKEN) {
            if (current().type == TokenType::NAMESPACE) {
                program->statements.push_back(parseNamespaceDeclaration());
            } else if (current().type == TokenType::IMPORT) {
                program->statements.push_back(parseImportStatement());
            } else if (current().type == TokenType::FN) {
                program->statements.push_back(parseFunctionDeclaration());
            } else if (current().type == TokenType::LET) {
                program->statements.push_back(parseVariableDeclaration());
            } else {
                program->statements.push_back(parseStatement());
            }
            skipNewlines();
        }
        
        return program;
    }
    
    std::unique_ptr<ASTNode> parseNamespaceDeclaration() {
        auto ns = std::make_unique<NamespaceDeclaration>();
        
        expect(TokenType::NAMESPACE);
        if (current().type == TokenType::IDENTIFIER) {
            ns->name = current().value;
            pos++;
        }
        
        skipNewlines();
        expect(TokenType::LBRACE);
        skipNewlines();
        
        auto block = std::make_unique<BlockStatement>();
        while (current().type != TokenType::RBRACE && current().type != TokenType::EOF_TOKEN) {
            block->statements.push_back(parseStatement());
            skipNewlines();
        }
        
        expect(TokenType::RBRACE);
        ns->body = std::move(block);
        
        return std::move(ns);
    }
    
    std::unique_ptr<ASTNode> parseImportStatement() {
        expect(TokenType::IMPORT);
        // 간단한 import 구현
        if (current().type == TokenType::STRING_LITERAL) {
            auto importStmt = std::make_unique<StringLiteral>(current().value);
            pos++;
            return std::move(importStmt);
        }
        return nullptr;
    }
    
    std::unique_ptr<ASTNode> parseFunctionDeclaration() {
        auto func = std::make_unique<FunctionDeclaration>();
        
        expect(TokenType::FN);
        
        if (current().type == TokenType::IDENTIFIER) {
            func->name = current().value;
            pos++;
        }
        
        expect(TokenType::LPAREN);
        
        // 매개변수 파싱
        while (current().type != TokenType::RPAREN && current().type != TokenType::EOF_TOKEN) {
            if (isDataType(current().type)) {
                std::string paramType = current().value;
                pos++;
                
                if (current().type == TokenType::IDENTIFIER) {
                    std::string paramName = current().value;
                    pos++;
                    func->parameters.emplace_back(paramType, paramName);
                }
                
                if (!match(TokenType::COMMA)) break;
            } else {
                break;
            }
        }
        
        expect(TokenType::RPAREN);
        
        // 반환 타입 (선택적)
        if (current().type == TokenType::SEMICOLON) {
            pos++;
            func->returnType = "void";
            return std::move(func);
        }
        
        skipNewlines();
        
        if (current().type == TokenType::LBRACE) {
            func->body = parseBlockStatement();
        }
        
        return std::move(func);
    }
    
    std::unique_ptr<ASTNode> parseVariableDeclaration() {
        auto var = std::make_unique<VariableDeclaration>();
        
        expect(TokenType::LET);
        
        if (current().type == TokenType::IDENTIFIER) {
            var->name = current().value;
            pos++;
        }
        
        if (match(TokenType::COMMA)) {
            // 타입 지정
            if (isDataType(current().type)) {
                var->dataType = current().value;
                pos++;
            }
        }
        
        if (match(TokenType::ASSIGN)) {
            var->initializer = parseExpression();
        }
        
        expect(TokenType::SEMICOLON);
        
        return std::move(var);
    }
    
    std::unique_ptr<ASTNode> parseStatement() {
        skipNewlines();
        
        switch (current().type) {
            case TokenType::LBRACE:
                return parseBlockStatement();
            case TokenType::IF:
                return parseIfStatement();
            case TokenType::WHILE:
                return parseWhileStatement();
            case TokenType::RETURN:
                return parseReturnStatement();
            case TokenType::LET:
                return parseVariableDeclaration();
            default:
                return parseExpressionStatement();
        }
    }
    
    std::unique_ptr<ASTNode> parseBlockStatement() {
        auto block = std::make_unique<BlockStatement>();
        
        expect(TokenType::LBRACE);
        skipNewlines();
        
        while (current().type != TokenType::RBRACE && current().type != TokenType::EOF_TOKEN) {
            block->statements.push_back(parseStatement());
            skipNewlines();
        }
        
        expect(TokenType::RBRACE);
        
        return std::move(block);
    }
    
    std::unique_ptr<ASTNode> parseIfStatement() {
        auto ifStmt = std::make_unique<IfStatement>();
        
        expect(TokenType::IF);
        expect(TokenType::LPAREN);
        ifStmt->condition = parseExpression();
        expect(TokenType::RPAREN);
        
        skipNewlines();
        ifStmt->thenStatement = parseStatement();
        
        return std::move(ifStmt);
    }
    
    std::unique_ptr<ASTNode> parseWhileStatement() {
        auto whileStmt = std::make_unique<WhileStatement>();
        
        expect(TokenType::WHILE);
        expect(TokenType::LPAREN);
        whileStmt->condition = parseExpression();
        expect(TokenType::RPAREN);
        
        skipNewlines();
        whileStmt->body = parseStatement();
        
        return std::move(whileStmt);
    }
    
    std::unique_ptr<ASTNode> parseReturnStatement() {
        auto returnStmt = std::make_unique<ReturnStatement>();
        
        expect(TokenType::RETURN);
        
        if (current().type != TokenType::SEMICOLON) {
            returnStmt->expression = parseExpression();
        }
        
        expect(TokenType::SEMICOLON);
        
        return std::move(returnStmt);
    }
    
    std::unique_ptr<ASTNode> parseExpressionStatement() {
        auto exprStmt = std::make_unique<ExpressionStatement>();
        exprStmt->expression = parseExpression();
        expect(TokenType::SEMICOLON);
        return std::move(exprStmt);
    }
    
    bool isDataType(TokenType type) {
        return type == TokenType::INT || type == TokenType::FLOAT ||
               type == TokenType::CHAR || type == TokenType::BYTE ||
               type == TokenType::LONG || type == TokenType::DOUBLE ||
               type == TokenType::SHORT || type == TokenType::BOOL ||
               type == TokenType::STRING || type == TokenType::VOID;
    }
};

// 표현식 파싱 메서드들
std::unique_ptr<ASTNode> Parser::parseExpression() {
    return parseAssignmentExpression();
}

std::unique_ptr<ASTNode> Parser::parseAssignmentExpression() {
    auto expr = parseLogicalOrExpression();
    
    if (match(TokenType::ASSIGN)) {
        auto assignment = std::make_unique<AssignmentExpression>();
        assignment->left = std::move(expr);
        assignment->right = parseAssignmentExpression();
        return std::move(assignment);
    }
    
    return expr;
}

std::unique_ptr<ASTNode> Parser::parseLogicalOrExpression() {
    auto left = parseLogicalAndExpression();
    
    while (current().type == TokenType::LOGICAL_OR) {
        TokenType op = current().type;
        pos++;
        auto right = parseLogicalAndExpression();
        
        auto binary = std::make_unique<BinaryExpression>();
        binary->left = std::move(left);
        binary->operator_ = op;
        binary->right = std::move(right);
        left = std::move(binary);
    }
    
    return left;
}

std::unique_ptr<ASTNode> Parser::parseLogicalAndExpression() {
    auto left = parseBitwiseOrExpression();
    
    while (current().type == TokenType::LOGICAL_AND) {
        TokenType op = current().type;
        pos++;
        auto right = parseBitwiseOrExpression();
        
        auto binary = std::make_unique<BinaryExpression>();
        binary->left = std::move(left);
        binary->operator_ = op;
        binary->right = std::move(right);
        left = std::move(binary);
    }
    
    return left;
}

std::unique_ptr<ASTNode> Parser::parseBitwiseOrExpression() {
    auto left = parseBitwiseXorExpression();
    
    while (current().type == TokenType::BIT_OR) {
        TokenType op = current().type;
        pos++;
        auto right = parseBitwiseXorExpression();
        
        auto binary = std::make_unique<BinaryExpression>();
        binary->left = std::move(left);
        binary->operator_ = op;
        binary->right = std::move(right);
        left = std::move(binary);
    }
    
    return left;
}

std::unique_ptr<ASTNode> Parser::parseBitwiseXorExpression() {
    auto left = parseBitwiseAndExpression();
    
    while (current().type == TokenType::BIT_XOR) {
        TokenType op = current().type;
        pos++;
        auto right = parseBitwiseAndExpression();
        
        auto binary = std::make_unique<BinaryExpression>();
        binary->left = std::move(left);
        binary->operator_ = op;
        binary->right = std::move(right);
        left = std::move(binary);
    }
    
    return left;
}

std::unique_ptr<ASTNode> Parser::parseBitwiseAndExpression() {
    auto left = parseEqualityExpression();
    
    while (current().type == TokenType::BIT_AND) {
        TokenType op = current().type;
        pos++;
        auto right = parseEqualityExpression();
        
        auto binary = std::make_unique<BinaryExpression>();
        binary->left = std::move(left);
        binary->operator_ = op;
        binary->right = std::move(right);
        left = std::move(binary);
    }
    
    return left;
}

std::unique_ptr<ASTNode> Parser::parseEqualityExpression() {
    auto left = parseRelationalExpression();
    
    while (current().type == TokenType::EQUAL || current().type == TokenType::NOT_EQUAL) {
        TokenType op = current().type;
        pos++;
        auto right = parseRelationalExpression();
        
        auto binary = std::make_unique<BinaryExpression>();
        binary->left = std::move(left);
        binary->operator_ = op;
        binary->right = std::move(right);
        left = std::move(binary);
    }
    
    return left;
}

std::unique_ptr<ASTNode> Parser::parseRelationalExpression() {
    auto left = parseShiftExpression();
    
    while (current().type == TokenType::LESS || current().type == TokenType::GREATER ||
           current().type == TokenType::LESS_EQUAL || current().type == TokenType::GREATER_EQUAL) {
        TokenType op = current().type;
        pos++;
        auto right = parseShiftExpression();
        
        auto binary = std::make_unique<BinaryExpression>();
        binary->left = std::move(left);
        binary->operator_ = op;
        binary->right = std::move(right);
        left = std::move(binary);
    }
    
    return left;
}

std::unique_ptr<ASTNode> Parser::parseShiftExpression() {
    auto left = parseAdditiveExpression();
    
    while (current().type == TokenType::LEFT_SHIFT || current().type == TokenType::RIGHT_SHIFT) {
        TokenType op = current().type;
        pos++;
        auto right = parseAdditiveExpression();
        
        auto binary = std::make_unique<BinaryExpression>();
        binary->left = std::move(left);
        binary->operator_ = op;
        binary->right = std::move(right);
        left = std::move(binary);
    }
    
    return left;
}

std::unique_ptr<ASTNode> Parser::parseAdditiveExpression() {
    auto left = parseMultiplicativeExpression();
    
    while (current().type == TokenType::PLUS || current().type == TokenType::MINUS) {
        TokenType op = current().type;
        pos++;
        auto right = parseMultiplicativeExpression();
        
        auto binary = std::make_unique<BinaryExpression>();
        binary->left = std::move(left);
        binary->operator_ = op;
        binary->right = std::move(right);
        left = std::move(binary);
    }
    
    return left;
}

std::unique_ptr<ASTNode> Parser::parseMultiplicativeExpression() {
    auto left = parseUnaryExpression();
    
    while (current().type == TokenType::MULTIPLY || current().type == TokenType::DIVIDE || 
           current().type == TokenType::MODULO) {
        TokenType op = current().type;
        pos++;
        auto right = parseUnaryExpression();
        
        auto binary = std::make_unique<BinaryExpression>();
        binary->left = std::move(left);
        binary->operator_ = op;
        binary->right = std::move(right);
        left = std::move(binary);
    }
    
    return left;
}

std::unique_ptr<ASTNode> Parser::parseUnaryExpression() {
    if (current().type == TokenType::LOGICAL_NOT || current().type == TokenType::BIT_NOT ||
        current().type == TokenType::MINUS || current().type == TokenType::PLUS) {
        
        auto unary = std::make_unique<UnaryExpression>();
        unary->operator_ = current().type;
        pos++;
        unary->operand = parseUnaryExpression();
        return std::move(unary);
    }
    
    return parsePostfixExpression();
}

std::unique_ptr<ASTNode> Parser::parsePostfixExpression() {
    auto expr = parsePrimaryExpression();
    
    while (true) {
        if (current().type == TokenType::LPAREN) {
            // 함수 호출
            auto call = std::make_unique<CallExpression>();
            call->callee = std::move(expr);
            
            pos++; // '(' 건너뛰기
            
            while (current().type != TokenType::RPAREN && current().type != TokenType::EOF_TOKEN) {
                call->arguments.push_back(parseExpression());
                if (!match(TokenType::COMMA)) break;
            }
            
            expect(TokenType::RPAREN);
            expr = std::move(call);
        } else {
            break;
        }
    }
    
    return expr;
}

std::unique_ptr<ASTNode> Parser::parsePrimaryExpression() {
    switch (current().type) {
        case TokenType::INTEGER_LITERAL: {
            int value = std::stoi(current().value);
            pos++;
            return std::make_unique<IntegerLiteral>(value);
        }
        case TokenType::FLOAT_LITERAL: {
            double value = std::stod(current().value);
            pos++;
            return std::make_unique<FloatLiteral>(value);
        }
        case TokenType::STRING_LITERAL: {
            std::string value = current().value;
            pos++;
            return std::make_unique<StringLiteral>(value);
        }
        case TokenType::CHAR_LITERAL: {
            std::string value = current().value;
            pos++;
            return std::make_unique<StringLiteral>(value);
        }
        case TokenType::BOOL_LITERAL: {
            bool value = (current().value == "true");
            pos++;
            return std::make_unique<BoolLiteral>(value);
        }
        case TokenType::IDENTIFIER: {
            std::string name = current().value;
            pos++;
            return std::make_unique<Identifier>(name);
        }
        case TokenType::LPAREN: {
            pos++; // '(' 건너뛰기
            auto expr = parseExpression();
            expect(TokenType::RPAREN);
            return expr;
        }
        default:
            throw std::runtime_error("Unexpected token in primary expression at line " + 
                std::to_string(current().line));
    }
}

// ===== 심볼 테이블 및 타입 검사 =====
struct Symbol {
    std::string name;
    std::string type;
    bool isFunction;
    std::vector<std::string> paramTypes;


    Symbol() : name(""), type(""), isFunction(false) {}


    Symbol(const std::string& n, const std::string& t, bool func = false)
        : name(n), type(t), isFunction(func) {}
};


class SymbolTable {
private:
    std::vector<std::unordered_map<std::string, Symbol>> scopes;
    
public:
    SymbolTable() {
        pushScope(); // 전역 스코프
    }
    
    void pushScope() {
        scopes.emplace_back();
    }
    
    void popScope() {
        if (scopes.size() > 1) {
            scopes.pop_back();
        }
    }
    
    void declare(const Symbol& symbol) {
        if (scopes.back().find(symbol.name) != scopes.back().end()) {
            throw std::runtime_error("Variable '" + symbol.name + "' already declared in this scope");
        }
        scopes.back()[symbol.name] = symbol;
    }
    
    Symbol* lookup(const std::string& name) {
        for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
            auto found = it->find(name);
            if (found != it->end()) {
                return &found->second;
            }
        }
        return nullptr;
    }
};

// ===== 의미 분석기 =====
class SemanticAnalyzer {
private:
    SymbolTable symbolTable;
    
    std::string analyzeExpression(ASTNode* node) {
        if (!node) return "void";
        
        switch (node->type) {
            case NodeType::INTEGER_LITERAL:
                return "int";
            case NodeType::FLOAT_LITERAL:
                return "float";
            case NodeType::STRING_LITERAL:
                return "string";
            case NodeType::BOOL_LITERAL:
                return "bool";
            case NodeType::IDENTIFIER: {
                auto id = static_cast<Identifier*>(node);
                Symbol* symbol = symbolTable.lookup(id->name);
                if (!symbol) {
                    throw std::runtime_error("Undefined variable: " + id->name);
                }
                return symbol->type;
            }
            case NodeType::BINARY_EXPRESSION: {
                auto binary = static_cast<BinaryExpression*>(node);
                std::string leftType = analyzeExpression(binary->left.get());
                std::string rightType = analyzeExpression(binary->right.get());
                
                // 타입 호환성 검사
                if (leftType != rightType) {
                    throw std::runtime_error("Type mismatch in binary expression");
                }
                
                return leftType;
            }
            case NodeType::ASSIGNMENT_EXPRESSION: {
                auto assignment = static_cast<AssignmentExpression*>(node);
                std::string leftType = analyzeExpression(assignment->left.get());
                std::string rightType = analyzeExpression(assignment->right.get());
                
                if (leftType != rightType) {
                    throw std::runtime_error("Type mismatch in assignment");
                }
                
                return leftType;
            }
            case NodeType::CALL_EXPRESSION: {
                auto call = static_cast<CallExpression*>(node);
                if (call->callee->type == NodeType::IDENTIFIER) {
                    auto id = static_cast<Identifier*>(call->callee.get());
                    Symbol* symbol = symbolTable.lookup(id->name);
                    if (!symbol || !symbol->isFunction) {
                        throw std::runtime_error("Undefined function: " + id->name);
                    }
                    
                    // 매개변수 타입 검사
                    if (call->arguments.size() != symbol->paramTypes.size()) {
                        throw std::runtime_error("Argument count mismatch for function: " + id->name);
                    }
                    
                    for (size_t i = 0; i < call->arguments.size(); ++i) {
                        std::string argType = analyzeExpression(call->arguments[i].get());
                        if (argType != symbol->paramTypes[i]) {
                            throw std::runtime_error("Argument type mismatch for function: " + id->name);
                        }
                    }
                    
                    return symbol->type;
                }
                break;
            }
            default:
                break;
        }
        
        return "void";
    }
    
    void analyzeStatement(ASTNode* node) {
        if (!node) return;
        
        switch (node->type) {
            case NodeType::VARIABLE_DECLARATION: {
                auto var = static_cast<VariableDeclaration*>(node);
                
                if (var->initializer) {
                    std::string initType = analyzeExpression(var->initializer.get());
                    if (!var->dataType.empty() && var->dataType != initType) {
                        throw std::runtime_error("Type mismatch in variable declaration: " + var->name);
                    }
                    if (var->dataType.empty()) {
                        var->dataType = initType; // 타입 추론
                    }
                }
                
                symbolTable.declare(Symbol(var->name, var->dataType));
                break;
            }
            case NodeType::FUNCTION_DECLARATION: {
                auto func = static_cast<FunctionDeclaration*>(node);
                
                Symbol funcSymbol(func->name, func->returnType, true);
                for (const auto& param : func->parameters) {
                    funcSymbol.paramTypes.push_back(param.first);
                }
                
                symbolTable.declare(funcSymbol);
                
                // 함수 본문 분석
                symbolTable.pushScope();
                
                // 매개변수를 스코프에 추가
                for (const auto& param : func->parameters) {
                    symbolTable.declare(Symbol(param.second, param.first));
                }
                
                if (func->body) {
                    analyzeStatement(func->body.get());
                }
                
                symbolTable.popScope();
                break;
            }
            case NodeType::BLOCK_STATEMENT: {
                auto block = static_cast<BlockStatement*>(node);
                symbolTable.pushScope();
                
                for (const auto& stmt : block->statements) {
                    analyzeStatement(stmt.get());
                }
                
                symbolTable.popScope();
                break;
            }
            case NodeType::IF_STATEMENT: {
                auto ifStmt = static_cast<IfStatement*>(node);
                
                std::string condType = analyzeExpression(ifStmt->condition.get());
                if (condType != "bool") {
                    throw std::runtime_error("If condition must be boolean");
                }
                
                analyzeStatement(ifStmt->thenStatement.get());
                if (ifStmt->elseStatement) {
                    analyzeStatement(ifStmt->elseStatement.get());
                }
                break;
            }
            case NodeType::WHILE_STATEMENT: {
                auto whileStmt = static_cast<WhileStatement*>(node);
                
                std::string condType = analyzeExpression(whileStmt->condition.get());
                if (condType != "bool") {
                    throw std::runtime_error("While condition must be boolean");
                }
                
                analyzeStatement(whileStmt->body.get());
                break;
            }
            case NodeType::RETURN_STATEMENT: {
                auto returnStmt = static_cast<ReturnStatement*>(node);
                if (returnStmt->expression) {
                    analyzeExpression(returnStmt->expression.get());
                }
                break;
            }
            case NodeType::EXPRESSION_STATEMENT: {
                auto exprStmt = static_cast<ExpressionStatement*>(node);
                analyzeExpression(exprStmt->expression.get());
                break;
            }
            default:
                break;
        }
    }
    
public:
    void analyze(Program* program) {
        for (const auto& stmt : program->statements) {
            analyzeStatement(stmt.get());
        }
    }
};

// ===== 코드 생성기 =====
class CodeGenerator {
private:
    std::ostringstream output;
    int indentLevel = 0;
    
    void indent() {
        for (int i = 0; i < indentLevel; ++i) {
            output << "    ";
        }
    }
    
    void generateExpression(ASTNode* node) {
        if (!node) return;
        
        switch (node->type) {
            case NodeType::INTEGER_LITERAL: {
                auto lit = static_cast<IntegerLiteral*>(node);
                output << lit->value;
                break;
            }
            case NodeType::FLOAT_LITERAL: {
                auto lit = static_cast<FloatLiteral*>(node);
                output << lit->value;
                break;
            }
            case NodeType::STRING_LITERAL: {
                auto lit = static_cast<StringLiteral*>(node);
                output << "\"" << lit->value << "\"";
                break;
            }
            case NodeType::BOOL_LITERAL: {
                auto lit = static_cast<BoolLiteral*>(node);
                output << (lit->value ? "true" : "false");
                break;
            }
            case NodeType::IDENTIFIER: {
                auto id = static_cast<Identifier*>(node);
                output << id->name;
                break;
            }
            case NodeType::BINARY_EXPRESSION: {
                auto binary = static_cast<BinaryExpression*>(node);
                output << "(";
                generateExpression(binary->left.get());
                
                switch (binary->operator_) {
                    case TokenType::PLUS: output << " + "; break;
                    case TokenType::MINUS: output << " - "; break;
                    case TokenType::MULTIPLY: output << " * "; break;
                    case TokenType::DIVIDE: output << " / "; break;
                    case TokenType::MODULO: output << " % "; break;
                    case TokenType::EQUAL: output << " == "; break;
                    case TokenType::NOT_EQUAL: output << " != "; break;
                    case TokenType::LESS: output << " < "; break;
                    case TokenType::GREATER: output << " > "; break;
                    case TokenType::LESS_EQUAL: output << " <= "; break;
                    case TokenType::GREATER_EQUAL: output << " >= "; break;
                    case TokenType::LOGICAL_AND: output << " && "; break;
                    case TokenType::LOGICAL_OR: output << " || "; break;
                    case TokenType::BIT_AND: output << " & "; break;
                    case TokenType::BIT_OR: output << " | "; break;
                    case TokenType::BIT_XOR: output << " ^ "; break;
                    case TokenType::LEFT_SHIFT: output << " << "; break;
                    case TokenType::RIGHT_SHIFT: output << " >> "; break;
                    default: output << " OP "; break;
                }
                
                generateExpression(binary->right.get());
                output << ")";
                break;
            }
            case NodeType::ASSIGNMENT_EXPRESSION: {
                auto assignment = static_cast<AssignmentExpression*>(node);
                generateExpression(assignment->left.get());
                output << " = ";
                generateExpression(assignment->right.get());
                break;
            }
            case NodeType::CALL_EXPRESSION: {
                auto call = static_cast<CallExpression*>(node);
                generateExpression(call->callee.get());
                output << "(";
                
                for (size_t i = 0; i < call->arguments.size(); ++i) {
                    if (i > 0) output << ", ";
                    generateExpression(call->arguments[i].get());
                }
                
                output << ")";
                break;
            }
            default:
                break;
        }
    }
    
    void generateStatement(ASTNode* node) {
        if (!node) return;
        
        switch (node->type) {
            case NodeType::VARIABLE_DECLARATION: {
                auto var = static_cast<VariableDeclaration*>(node);
                indent();
                
                // C++ 타입 매핑
                std::string cppType = mapToCppType(var->dataType);
                output << cppType << " " << var->name;
                
                if (var->initializer) {
                    output << " = ";
                    generateExpression(var->initializer.get());
                }
                
                output << ";\n";
                break;
            }
            case NodeType::FUNCTION_DECLARATION: {
                auto func = static_cast<FunctionDeclaration*>(node);
                indent();
                
                std::string returnType = mapToCppType(func->returnType);
                output << returnType << " " << func->name << "(";
                
                for (size_t i = 0; i < func->parameters.size(); ++i) {
                    if (i > 0) output << ", ";
                    output << mapToCppType(func->parameters[i].first) << " " << func->parameters[i].second;
                }
                
                output << ")";
                
                if (func->body) {
                    output << " ";
                    generateStatement(func->body.get());
                } else {
                    output << ";\n";
                }
                break;
            }
            case NodeType::BLOCK_STATEMENT: {
                auto block = static_cast<BlockStatement*>(node);
                output << "{\n";
                indentLevel++;
                
                for (const auto& stmt : block->statements) {
                    generateStatement(stmt.get());
                }
                
                indentLevel--;
                indent();
                output << "}\n";
                break;
            }
            case NodeType::IF_STATEMENT: {
                auto ifStmt = static_cast<IfStatement*>(node);
                indent();
                output << "if (";
                generateExpression(ifStmt->condition.get());
                output << ") ";
                
                generateStatement(ifStmt->thenStatement.get());
                
                if (ifStmt->elseStatement) {
                    indent();
                    output << "else ";
                    generateStatement(ifStmt->elseStatement.get());
                }
                break;
            }
            case NodeType::WHILE_STATEMENT: {
                auto whileStmt = static_cast<WhileStatement*>(node);
                indent();
                output << "while (";
                generateExpression(whileStmt->condition.get());
                output << ") ";
                
                generateStatement(whileStmt->body.get());
                break;
            }
            case NodeType::RETURN_STATEMENT: {
                auto returnStmt = static_cast<ReturnStatement*>(node);
                indent();
                output << "return";
                
                if (returnStmt->expression) {
                    output << " ";
                    generateExpression(returnStmt->expression.get());
                }
                
                output << ";\n";
                break;
            }
            case NodeType::EXPRESSION_STATEMENT: {
                auto exprStmt = static_cast<ExpressionStatement*>(node);
                indent();
                generateExpression(exprStmt->expression.get());
                output << ";\n";
                break;
            }
            case NodeType::NAMESPACE_DECLARATION: {
                auto ns = static_cast<NamespaceDeclaration*>(node);
                indent();
                output << "namespace " << ns->name << " ";
                generateStatement(ns->body.get());
                break;
            }
            default:
                break;
        }
    }
    
    std::string mapToCppType(const std::string& type) {
        if (type == "int") return "int";
        if (type == "float") return "float";
        if (type == "double") return "double";
        if (type == "char") return "char";
        if (type == "bool") return "bool";
        if (type == "string") return "std::string";
        if (type == "byte") return "unsigned char";
        if (type == "short") return "short";
        if (type == "long") return "long";
        if (type == "void") return "void";
        return type;
    }
    
public:
    std::string generate(Program* program) {
        output << "#include <iostream>\n";
        output << "#include <string>\n";
        output << "#include <cmath>\n\n";
        
        for (const auto& stmt : program->statements) {
            generateStatement(stmt.get());
        }
        
        return output.str();
    }
};

// ===== 에러 처리 =====
class CompilerError : public std::exception {
public:
    enum Type { LEXICAL, SYNTAX, SEMANTIC, RUNTIME };
    
private:
    Type errorType;
    std::string message;
    int line;
    int column;
    
public:
    CompilerError(Type type, const std::string& msg, int l = 0, int c = 0)
        : errorType(type), message(msg), line(l), column(c) {}
    
    const char* what() const noexcept override {
        static std::string fullMessage;
        fullMessage = getTypeString() + " Error";
        if (line > 0) {
            fullMessage += " at line " + std::to_string(line);
            if (column > 0) {
                fullMessage += ", column " + std::to_string(column);
            }
        }
        fullMessage += ": " + message;
        return fullMessage.c_str();
    }
    
    std::string getTypeString() const {
        switch (errorType) {
            case LEXICAL: return "Lexical";
            case SYNTAX: return "Syntax";
            case SEMANTIC: return "Semantic";
            case RUNTIME: return "Runtime";
            default: return "Unknown";
        }
    }
};

// ===== 메인 컴파일러 클래스 =====
class Compiler {
public:
    std::string compile(const std::string& sourceCode) {
        try {
            // 1. 렉싱
            Lexer lexer(sourceCode);
            auto tokens = lexer.tokenize();
            
            // 2. 파싱
            Parser parser(tokens);
            auto ast = parser.parse();
            
            // 3. 의미 분석
            SemanticAnalyzer analyzer;
            analyzer.analyze(ast.get());
            
            // 4. 코드 생성
            CodeGenerator generator;
            return generator.generate(ast.get());
            
        } catch (const std::exception& e) {
            return "Compilation Error: " + std::string(e.what());
        }
    }
    
    void compileFile(const std::string& inputFile, const std::string& outputFile) {
        std::ifstream inFile(inputFile);
        if (!inFile) {
            throw std::runtime_error("Cannot open input file: " + inputFile);
        }
        
        std::string sourceCode((std::istreambuf_iterator<char>(inFile)),
                               std::istreambuf_iterator<char>());
        
        std::string result = compile(sourceCode);
        
        std::ofstream outFile(outputFile);
        if (!outFile) {
            throw std::runtime_error("Cannot create output file: " + outputFile);
        }
        
        outFile << result;
    }
};

// ===== 메인 함수 및 테스트 =====
int main(int argc, char* argv[]) {
    try {
        Compiler compiler;

        if (argc == 3) {
            // 파일 컴파일 모드
            compiler.compileFile(argv[1], argv[2]);
            std::cout << "Compilation successful: " << argv[1] << " -> " << argv[2] << std::endl;
        } else {
            // 테스트 모드
            std::string testCode = R"(
            fn main() {
                print(1); // 정수 출력
                print(3.14); // 실수 출력
                print("Hello, World!"); // 문자열 출력
                print(true); // 불리언 값 출력
            }
            )";

            std::string result = compiler.compile(testCode);
            
            // --- 코드 출력 순서 변경 및 "Generated C++ Code:" 처리 ---
            // 1. C++ 헤더 및 print 함수 오버로드 구현 먼저 출력
            std::cout << "#include <iostream>\n";
            std::cout << "#include <string>\n\n";
            std::cout << "void print(int value) { std::cout << value << std::endl; }\n";
            std::cout << "void print(double value) { std::cout << value << std::endl; }\n";
            std::cout << "void print(const std::string& value) { std::cout << value << std::endl; }\n";
            std::cout << "void print(bool value) { std::cout << (value ? \"true\" : \"false\") << std::endl; }\n\n";

            // 2. 선택적으로 주석 처리된 정보 추가 (원한다면)
            std::cout << "// Generated C++ Code:\n\n"; // 주석 처리하여 컴파일러가 무시하도록 함

            // 3. 실제 컴파일된 코드 (main 함수 등) 출력
            std::cout << result << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
