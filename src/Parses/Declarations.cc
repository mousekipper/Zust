#include <Parser.hh>
#include <new>
#include <cstdlib>
#include <memory>

#undef ccfn
#define ccfn Parser::

std::unique_ptr<ASTNode> ccfn parseNamespaceDeclaration() {
    
    auto ns = MkUniqueNode(NodeType::NAMESPACE_DECLARATION)();
    
    expect(TokenType::NAMESPACE);
    if (current().type == TokenType::IDENTIFIER) {
        ns->name = current().value;
        pos++;
    }
    
    skipNewlines();
    expect(TokenType::LBRACE);
    skipNewlines();

    auto block = MkUniqueNode(NodeType::BLOCK_STATEMENT)();
    while (current().type != TokenType::RBRACE && current().type != TokenType::EOF_TOKEN) {
        block->statements.push_back(parseStatement());
        skipNewlines();
    }

    // deadline
    
    expect(TokenType::RBRACE);
    ns->body = std::move(block);
    
    return std::move(ns);
}

std::unique_ptr<ASTNode> ccfn parseFunctionDeclaration() {
    auto func = MkUniqueNode(NodeType::FUNCTION_DECLARATION)();
    
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
        func->returnType = "auto";
        return std::move(func);
    } 
    
    if(current().type == TokenType::COLUMN)
    {
        pos++;
        if(!isDataType(current().type)) {
            throw std::runtime_error("[parseFunctionNode] expected dattype");
        }

        func->returnType = (current().value);
        pos++;
    } else {
        /**  Here type is not specified */
        func->returnType = "auto";
    }
    
    skipNewlines();
    
    if (current().type == TokenType::LBRACE) {
        func->body = parseBlockStatement();
    }
    
    return std::move(func);
}

std::unique_ptr<ASTNode> ccfn parseVariableDeclaration() {
    auto var = MkUniqueNode(NodeType::VARIABLE_DECLARATION)();
    
    expect(TokenType::LET);
    
    if (current().type == TokenType::IDENTIFIER) {
        var->name = current().value;
        pos++;
    }

    if (match(TokenType::COLUMN)) {
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

