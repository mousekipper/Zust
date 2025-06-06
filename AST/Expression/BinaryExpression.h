#ifndef BINARY_EXPRESSION_H
#define BINARY_EXPRESSION_H
#include "../ASTNode.h"
#include <memory>
#include "../TokenType.h"

struct BinaryExpression : ASTNode {
    std::unique_ptr<ASTNode> left;
    TokenType operator_;
    std::unique_ptr<ASTNode> right;
    
    BinaryExpression() { type = NodeType::BINARY_EXPRESSION; }
};

#endif // BINARY_EXPRESSION_H