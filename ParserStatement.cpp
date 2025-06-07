#include "Parser.h"


std::unique_ptr<ASTNode> Parser::parseImportStatement(){
    expect(TokenType::IMPORT);
    // 간단한 import 구현
    if (current().type == TokenType::STRING_LITERAL) {
        auto importStmt = std::make_unique<StringLiteral>(current().value);
        pos++;
        return std::move(importStmt);
    }
    return nullptr;
}
    
std::unique_ptr<ASTNode> Parser::parseStatement(){
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

std::unique_ptr<ASTNode> Parser::parseBlockStatement(){
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

std::unique_ptr<ASTNode> Parser::parseIfStatement(){
    auto ifStmt = std::make_unique<IfStatement>();
    
    expect(TokenType::IF);
    expect(TokenType::LPAREN);
    ifStmt->condition = parseExpression();
    expect(TokenType::RPAREN);
    
    skipNewlines();
    ifStmt->thenStatement = parseStatement();
    
    return std::move(ifStmt);
}

std::unique_ptr<ASTNode> Parser::parseWhileStatement() {
    auto whileStmt = std::make_unique<WhileStatement>();
    
    expect(TokenType::WHILE);
    expect(TokenType::LPAREN);
    whileStmt->condition = parseExpression();
    expect(TokenType::RPAREN);
    
    skipNewlines();
    whileStmt->body = parseStatement();
    
    return std::move(whileStmt);
}

std::unique_ptr<ASTNode> Parser::parseReturnStatement() {
    auto returnStmt = std::make_unique<ReturnStatement>();
    
    expect(TokenType::RETURN);
    
    if (current().type != TokenType::SEMICOLON) {
        returnStmt->expression = parseExpression();
    }
    
    expect(TokenType::SEMICOLON);
    
    return std::move(returnStmt);
}

std::unique_ptr<ASTNode> Parser::parseExpressionStatement() {
    auto exprStmt = std::make_unique<ExpressionStatement>();
    exprStmt->expression = parseExpression();
    expect(TokenType::SEMICOLON);
    return std::move(exprStmt);
}