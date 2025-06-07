#ifndef PARSER_H
#define PARSER_H
#include <memory>
#include "AST/AST.h"
#include <iostream>

#include "Program.h"

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
    
    std::unique_ptr<ASTNode> parseNamespaceDeclaration();
    
    std::unique_ptr<ASTNode> parseImportStatement();
    std::unique_ptr<ASTNode> parseFunctionDeclaration();
    
    std::unique_ptr<ASTNode> parseVariableDeclaration();
    
    std::unique_ptr<ASTNode> parseStatement();
    
    std::unique_ptr<ASTNode> parseBlockStatement();
    
    std::unique_ptr<ASTNode> parseIfStatement();
    
    std::unique_ptr<ASTNode> parseWhileStatement();
    
    std::unique_ptr<ASTNode> parseReturnStatement();
    
    std::unique_ptr<ASTNode> parseExpressionStatement();
    
    bool isDataType(TokenType type) {
        return type == TokenType::INT || type == TokenType::FLOAT ||
               type == TokenType::CHAR || type == TokenType::BYTE ||
               type == TokenType::LONG || type == TokenType::DOUBLE ||
               type == TokenType::SHORT || type == TokenType::BOOL ||
               type == TokenType::STRING || type == TokenType::VOID;
    }
};
#endif // PARSER_H