#include <iostream>
#include <vector>
#include <string>
#include <unordered_set>
#include <cctype>
#include <memory>
#include <stdexcept>

enum class TokenType {
    Keyword,
    Identifier,
    Number,
    Operator,
    Punctuation,
    StringLiteral,
    EndOfFile
};

struct Token {
    TokenType type;
    std::string text;
    int line;
    int column;
};

class Lexer {
public:
    Lexer(const std::string& source)
        : src(source), pos(0), line(1), column(1) {}

    std::vector<Token> tokenize() {
        std::vector<Token> tokens;
        while (true) {
            Token token = nextToken();
            tokens.push_back(token);
            if (token.type == TokenType::EndOfFile) break;
        }
        return tokens;
    }

private:
    std::string src;
    size_t pos;
    int line;
    int column;

    static const std::unordered_set<std::string> keywords;

    char peek() {
        return pos < src.size() ? src[pos] : '\0';
    }

    char get() {
        char c = peek();
        if (c == '\n') {
            line++;
            column = 1;
        } else {
            column++;
        }
        pos++;
        return c;
    }

    void skipWhitespace() {
        while (isspace(peek())) get();
    }

    bool isKeyword(const std::string& word) {
        return keywords.find(word) != keywords.end();
    }

    Token nextToken() {
        skipWhitespace();
        char c = peek();
        if (c == '\0') {
            return {TokenType::EndOfFile, "", line, column};
        }

        if (isalpha(c) || c == '_') {
            std::string word;
            while (isalnum(peek()) || peek() == '_') word += get();
            if (isKeyword(word)) {
                return {TokenType::Keyword, word, line, column};
            } else {
                return {TokenType::Identifier, word, line, column};
            }
        }

        if (isdigit(c)) {
            std::string number;
            while (isdigit(peek())) number += get();
            return {TokenType::Number, number, line, column};
        }

        if (ispunct(c)) {
            std::string op;
            op += get();
            return {TokenType::Operator, op, line, column};
        }

        get(); // skip unknown
        return nextToken();
    }
};

const std::unordered_set<std::string> Lexer::keywords = {
    "let", "fn", "if", "while", "for", "foreach", "switch",
    "namespace", "import", "int", "float", "char", "byte",
    "long", "double", "short", "bool"
};

//////////////////////////////////////////////////////
// AST Nodes

struct ASTNode {
    virtual ~ASTNode() = default;
    virtual void print(int indent = 0) const = 0;
};

struct VarDeclNode : ASTNode {
    std::string type;
    std::string name;
    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "VarDecl: " << type << " " << name << std::endl;
    }
};

struct FuncDeclNode : ASTNode {
    std::string name;
    std::vector<std::string> parameters;
    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "FuncDecl: " << name << "(";
        for (size_t i = 0; i < parameters.size(); ++i) {
            std::cout << parameters[i];
            if (i < parameters.size() - 1) std::cout << ", ";
        }
        std::cout << ")" << std::endl;
    }
};

struct BlockNode : ASTNode {
    std::vector<std::unique_ptr<ASTNode>> statements;
    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "Block:" << std::endl;
        for (const auto& stmt : statements) {
            stmt->print(indent + 2);
        }
    }
};

//////////////////////////////////////////////////////
// Parser

class Parser {
public:
    Parser(const std::vector<Token>& tokens)
        : tokens(tokens), pos(0) {}

    std::unique_ptr<BlockNode> parse() {
        auto block = std::make_unique<BlockNode>();
        while (peek().type != TokenType::EndOfFile) {
            block->statements.push_back(parseStatement());
        }
        return block;
    }

private:
    const std::vector<Token>& tokens;
    size_t pos;

    const Token& peek() {
        return tokens[pos];
    }

    const Token& get() {
        return tokens[pos++];
    }

    bool match(TokenType type, const std::string& text = "") {
        if (peek().type != type) return false;
        if (!text.empty() && peek().text != text) return false;
        pos++;
        return true;
    }

    std::unique_ptr<ASTNode> parseStatement() {
        if (match(TokenType::Keyword, "let")) {
            return parseVarDecl();
        }
        if (match(TokenType::Keyword, "fn")) {
            return parseFuncDecl();
        }
        throw std::runtime_error("Unknown statement at line " + std::to_string(peek().line));
    }

    std::unique_ptr<ASTNode> parseVarDecl() {
        if (peek().type != TokenType::Keyword)
            throw std::runtime_error("Expected type after 'let'");
        std::string type = get().text;

        if (peek().type != TokenType::Identifier)
            throw std::runtime_error("Expected identifier after type");

        std::string name = get().text;

        if (!match(TokenType::Operator, ";"))
            throw std::runtime_error("Expected ';' after variable declaration");

        return std::make_unique<VarDeclNode>(VarDeclNode{type, name});
    }

    std::unique_ptr<ASTNode> parseFuncDecl() {
        if (peek().type != TokenType::Identifier)
            throw std::runtime_error("Expected function name after 'fn'");
        std::string name = get().text;

        if (!match(TokenType::Operator, "("))
            throw std::runtime_error("Expected '(' after function name");

        std::vector<std::string> params;
        while (!match(TokenType::Operator, ")")) {
            if (peek().type == TokenType::Identifier) {
                params.push_back(get().text);
                match(TokenType::Operator, ",");
            } else {
                throw std::runtime_error("Unexpected token in parameter list");
            }
        }

        if (!match(TokenType::Operator, "{"))
            throw std::runtime_error("Expected '{' after function signature");

        // For now, skip parsing function body, just skip till '}'
        int braceCount = 1;
        while (braceCount > 0) {
            if (peek().type == TokenType::Operator && peek().text == "{") {
                get();
                braceCount++;
            } else if (peek().type == TokenType::Operator && peek().text == "}") {
                get();
                braceCount--;
            } else {
                get();
            }
        }

        return std::make_unique<FuncDeclNode>(FuncDeclNode{name, params});
    }
};

//////////////////////////////////////////////////////
// Main

int main() {
    std::string sourceCode = R"(
        let int x;
        fn myFunc(a, b) {
            let float y;
        }
    )";

    try {
        Lexer lexer(sourceCode);
        auto tokens = lexer.tokenize();

        Parser parser(tokens);
        auto ast = parser.parse();

        ast->print();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
