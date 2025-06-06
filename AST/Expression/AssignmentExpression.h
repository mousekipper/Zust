#ifndef AST_ASSIGNMENT_EXPRESSION_H
#define AST_ASSIGNMENT_EXPRESSION_H

#include "../ASTNode.h"
#include <memory>

struct AssignmentExpression : ASTNode {
    std::unique_ptr<ASTNode> left;
    std::unique_ptr<ASTNode> right;
    
    AssignmentExpression() { type = NodeType::ASSIGNMENT_EXPRESSION; }
};
#endif // AST_ASSIGNMENT_EXPRESSION_H