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
#include <stack>

// ===== 토큰 정의 =====
enum class TokenType {
    // 키워드
    LET, FN, IF, WHILE, FOR, FOREACH, SWITCH, CASE, DEFAULT,
    NAMESPACE, IMPORT, RETURN, BREAK, CONTINUE, ELSE,

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
    COLON, // 추가됨
    LPAREN, RPAREN, LBRACE, RBRACE, LBRACKET, RBRACKET,

    // 리터럴
    INTEGER_LITERAL, FLOAT_LITERAL, CHAR_LITERAL, STRING_LITERAL,
    BOOL_LITERAL, NIL_LITERAL,

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
            {"true", TokenType::BOOL_LITERAL}, {"false", TokenType::BOOL_LITERAL},
            {"nil", TokenType::NIL_LITERAL}
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
        while (peek() != '\n' && peek() != '\0') {
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
            } else if (isdigit(c)) {
                tokens.push_back(readNumber());
            } else if (isalpha(c) || c == '_') {
                tokens.push_back(readIdentifier());
            } else if (c == '#') {
                advance();
                tokens.push_back(readComment());
            } else if (c == '"') {
                tokens.push_back(readString());
            } else {
                switch (c) {
                    case ':': advance(); tokens.emplace_back(TokenType::COLON, ":", startLine, startCol); break;
                    case ';': advance(); tokens.emplace_back(TokenType::SEMICOLON, ";", startLine, startCol); break;
                    case ',': advance(); tokens.emplace_back(TokenType::COMMA, ",", startLine, startCol); break;
                    // 다른 연산자 추가 처리
                    default:
                        tokens.emplace_back(TokenType::UNKNOWN, std::string(1, advance()), startLine, startCol);
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
    CHAR_LITERAL, BOOL_LITERAL, NIL_LITERAL,             // NIL 리터럴 추가
    ASSIGNMENT_EXPRESSION, NAMESPACE_DECLARATION, IMPORT_STATEMENT
};

struct ASTNode {
    NodeType type;
    virtual ~ASTNode() = default;
};

struct NilLiteral : ASTNode {
    NilLiteral() { type = NodeType::NIL_LITERAL; }
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
    std::vector<std::pair<std::string, std::string>> parameters; // pair<type, name>
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
class Parser {
private:
    std::vector<Token> tokens;
    size_t pos;

    Token& current() {
        if (pos >= tokens.size()) return tokens.back(); // EOF 토큰
        return tokens[pos];
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
            throw std::runtime_error("Expected token type but got " + current().value);
        }
    }

    std::unique_ptr<ASTNode> parseExpression();

public:
    Parser(const std::vector<Token>& toks) : tokens(toks), pos(0) {}

    std::unique_ptr<Program> parse();

    std::unique_ptr<ASTNode> parseBlockStatement();
    std::unique_ptr<ASTNode> parseIfStatement();
};

// ===== Parser 구현부 =====
std::unique_ptr<Program> Parser::parse() {
    auto program = std::make_unique<Program>();

    while (current().type != TokenType::EOF_TOKEN) {
        program->statements.push_back(parseBlockStatement()); 
    }

    return program;
}

std::unique_ptr<ASTNode> Parser::parseBlockStatement() {
    auto block = std::make_unique<BlockStatement>();
    expect(TokenType::LBRACE);
    while (current().type != TokenType::RBRACE) {
        block->statements.push_back(parseIfStatement());
    }
    expect(TokenType::RBRACE);
    return block;
}

std::unique_ptr<ASTNode> Parser::parseIfStatement() {
    auto ifStmt = std::make_unique<IfStatement>();
    expect(TokenType::IF);
    expect(TokenType::LPAREN);
    ifStmt->condition = parseExpression();
    expect(TokenType::RPAREN);
    ifStmt->thenStatement = parseBlockStatement();
    if (match(TokenType::ELSE)) {
        ifStmt->elseStatement = parseBlockStatement();
    }
    return ifStmt;


};
class SemanticAnalyzer {
private:
    std::unordered_map<std::string, std::string> symbolTable;

public:
    void analyze(ASTNode* node) {
        // 분석 로직 구현 (타입 검사, 변수 이름 확인 등)
        if (!node) return;

        switch (node->type) {
            case NodeType::VARIABLE_DECLARATION: {
                auto varDecl = static_cast<VariableDeclaration*>(node);
                if (symbolTable.count(varDecl->name)) {
                    throw std::runtime_error("Variable already declared: " + varDecl->name);
                }
                symbolTable[varDecl->name] = varDecl->dataType;
                break;
            }
            case NodeType::FUNCTION_DECLARATION: {
                auto funcDecl = static_cast<FunctionDeclaration*>(node);
                if (symbolTable.count(funcDecl->name)) {
                    throw std::runtime_error("Function already declared: " + funcDecl->name);
                }
                symbolTable[funcDecl->name] = funcDecl->returnType;
                break;
            }
            default:
                break;
        }
    }
};
class CodeGenerator {
private:
    std::ostringstream output;

public:
    void generate(ASTNode* node) {
        // 코드 생성 로직
        if (!node) return;

        switch (node->type) {
            case NodeType::VARIABLE_DECLARATION: {
                auto varDecl = static_cast<VariableDeclaration*>(node);
                output << varDecl->dataType << " " << varDecl->name;
                if (varDecl->initializer) {
                    output << " = ";
                    generate(varDecl->initializer.get());
                }
                output << ";\n";
                break;
            }
            default:
                break;
        }
    }

    std::string getResult() {
        return output.str();
    }
};
class Compiler {
public:
    std::string compile(const std::string& source) {
        Lexer lexer(source);
        auto tokens = lexer.tokenize();

        Parser parser(tokens);
        auto ast = parser.parse();

        SemanticAnalyzer analyzer;
        for (const auto& stmt : ast->statements) {
            analyzer.analyze(stmt.get());
        }

        CodeGenerator generator;
        for (const auto& stmt : ast->statements) {
            generator.generate(stmt.get());
        }

        return generator.getResult();
    }
};

int main() {
    Compiler compiler;
    std::string source = R"(
        fn main() {
            let x: int = 5;
            let y: float = 3.14;
        }
    )";

    try {
        std::string result = compiler.compile(source);
        std::cout << "Generated Code:\n" << result << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Compilation Error: " << e.what() << std::endl;
    }

    return 0;
}
