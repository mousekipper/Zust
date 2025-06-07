#include "Parser.h"

std::unique_ptr<ASTNode> Parser::parseNamespaceDeclaration() {
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
    

std::unique_ptr<ASTNode> Parser::parseFunctionDeclaration() {
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

    std::unique_ptr<ASTNode> Parser::parseVariableDeclaration() {
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