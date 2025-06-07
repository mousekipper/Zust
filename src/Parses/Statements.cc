#include <Parser.hh>
#include <new>
#include <cstdlib>
#include <memory>

#undef ccfn
#define ccfn Parser::

std::unique_ptr<ASTNode> ccfn parseWhileStatement() {
    auto whileStmt = MkUniqueNode(NodeType::WHILE_STATEMENT)();
    
    expect(TokenType::WHILE);
    expect(TokenType::LPAREN);
    whileStmt->condition = parseExpression();
    expect(TokenType::RPAREN);
    
    skipNewlines();
    whileStmt->body = parseStatement();
    
    return std::move(whileStmt);
}

std::unique_ptr<ASTNode> ccfn parseReturnStatement() {
    auto returnStmt = MkUniqueNode(NodeType::RETURN_STATEMENT)();
    
    expect(TokenType::RETURN);
    
    if (current().type != TokenType::SEMICOLON) {
        returnStmt->expression = parseExpression();
    }
    
    expect(TokenType::SEMICOLON);
    
    return std::move(returnStmt);
}

std::unique_ptr<ASTNode> ccfn parseExpressionStatement() {
    auto exprStmt = MkUniqueNode(NodeType::EXPRESSION_STATEMENT)();
    exprStmt->expression = parseExpression();
    expect(TokenType::SEMICOLON);
    return std::move(exprStmt);
}

std::unique_ptr<ASTNode> ccfn parseStatement() {
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

std::unique_ptr<ASTNode> ccfn parseBlockStatement() {
    auto block = MkUniqueNode(NodeType::BLOCK_STATEMENT)();
    
    expect(TokenType::LBRACE);
    skipNewlines();
    
    while (current().type != TokenType::RBRACE && current().type != TokenType::EOF_TOKEN) {
        block->statements.push_back(parseStatement());
        skipNewlines();
    }
    
    expect(TokenType::RBRACE);
    
    return std::move(block);
}

std::unique_ptr<ASTNode> ccfn parseIfStatement() {
    auto ifStmt = MkUniqueNode(NodeType::IF_STATEMENT)();
    
    expect(TokenType::IF);
    expect(TokenType::LPAREN);
    ifStmt->condition = parseExpression();
    expect(TokenType::RPAREN);
    
    skipNewlines();
    ifStmt->thenStatement = parseStatement();
    
    return std::move(ifStmt);
}

std::unique_ptr<ASTNode> ccfn parseImportStatement() {
    expect(TokenType::IMPORT);
    // 간단한 import 구현
    if (current().type == TokenType::STRING_LITERAL) {
        auto importStmt = MkUniqueNode(NodeType::STRING_LITERAL)(current().value);
        pos++;
        return std::move(importStmt);
    }
    return nullptr;
}


