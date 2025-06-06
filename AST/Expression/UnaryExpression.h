#ifndef UNARY_EXPRESSION_H
#define UNARY_EXPRESSION_H
#include "../ASTNode.h"
#include "../TokenType.h"
#include <memory>

struct UnaryExpression : ASTNode {
    TokenType operator_;
    std::unique_ptr<ASTNode> operand;
    
    UnaryExpression() { type = NodeType::UNARY_EXPRESSION; }
};
#endif // UNARY_EXPRESSION_H