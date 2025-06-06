#include "Parser.h"
#include <string>
#include <memory>
#include <stdexcept>

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
