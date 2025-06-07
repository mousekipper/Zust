#ifndef Parser_hh
#define Parser_hh

#include "./Token.hh"
#include "./ASTNode.hh"
#include <vector>
#include <memory>
#include <stdexcept>
#include "./Nodes.hh"
#include "./Program.hh"

#define MkUniqueNode(e) std::make_unique<Node<e>>
#define ccfn

// ===== 파서 (Parser) =====
class Parser {
private:
    std::vector<Token> tokens;
    size_t pos;
    
    const inline Token& current() const {
        if (pos >= tokens.size()) return tokens.back(); // EOF 토큰
        return tokens[pos];
    }

    inline Token& current() {
        if (pos >= tokens.size()) return tokens.back(); // EOF 토큰
        return tokens[pos];
    }
    
    const Token& peek(int offset = 1) const {
        if (pos + offset >= tokens.size()) return tokens.back();
        return tokens[pos + offset];
    }

    Token& peek(int offset = 1) {
        if (pos + offset >= tokens.size()) return tokens.back();
        return tokens[pos + offset];
    }


    bool ccfn match(TokenType type, bool skip = 1);
    void ccfn expect(TokenType type);
    void ccfn skipNewlines();

    
    std::unique_ptr<ASTNode> ccfn parseExpression();
    std::unique_ptr<ASTNode> ccfn parseAssignmentExpression();
    std::unique_ptr<ASTNode> ccfn parseLogicalOrExpression();
    std::unique_ptr<ASTNode> ccfn parseLogicalAndExpression();
    std::unique_ptr<ASTNode> ccfn parseBitwiseOrExpression();
    std::unique_ptr<ASTNode> ccfn parseBitwiseXorExpression();
    std::unique_ptr<ASTNode> ccfn parseBitwiseAndExpression();
    std::unique_ptr<ASTNode> ccfn parseEqualityExpression();
    std::unique_ptr<ASTNode> ccfn parseRelationalExpression();
    std::unique_ptr<ASTNode> ccfn parseShiftExpression();
    std::unique_ptr<ASTNode> ccfn parseAdditiveExpression();
    std::unique_ptr<ASTNode> ccfn parseMultiplicativeExpression();
    std::unique_ptr<ASTNode> ccfn parseUnaryExpression();
    std::unique_ptr<ASTNode> ccfn parsePostfixExpression();
    std::unique_ptr<ASTNode> ccfn parsePrimaryExpression();
    
public:
    inline Parser(const std::vector<Token>& toks) : tokens(toks), pos(0) {}
    
    std::unique_ptr<Program> ccfn parse();
    std::unique_ptr<ASTNode> ccfn parseNamespaceDeclaration();    
    std::unique_ptr<ASTNode> ccfn parseImportStatement();
    std::unique_ptr<ASTNode> ccfn parseFunctionDeclaration();
    std::unique_ptr<ASTNode> ccfn parseVariableDeclaration();
    std::unique_ptr<ASTNode> ccfn parseStatement();
    std::unique_ptr<ASTNode> ccfn parseBlockStatement();
    std::unique_ptr<ASTNode> ccfn parseIfStatement();
    std::unique_ptr<ASTNode> ccfn parseWhileStatement();


    std::unique_ptr<ASTNode> ccfn parseReturnStatement();

    
    std::unique_ptr<ASTNode> ccfn parseExpressionStatement();

    
    bool isDataType(TokenType type) {
        return type == TokenType::INT || type == TokenType::FLOAT ||
               type == TokenType::CHAR || type == TokenType::BYTE ||
               type == TokenType::LONG || type == TokenType::DOUBLE ||
               type == TokenType::SHORT || type == TokenType::BOOL ||
               type == TokenType::STRING || type == TokenType::VOID;
    }
};

#endif