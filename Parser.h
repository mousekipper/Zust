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
        if (current().type == TokenType::COLON) {
            expect(TokenType::COLON);
            if (isDataType(current().type)) {
                func->returnType = current().value;
                pos++;
            }
        }else{
            func->returnType = "auto"; // 기본 반환 타입
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
        
        if (match(TokenType::COLON)) {
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
            case TokenType::FN:
                return parseFunctionDeclaration();
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
#endif // PARSER_H