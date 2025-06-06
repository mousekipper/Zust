#include "../ASTNode.h"
#include "../TokenType.h"
#include <memory>

struct UnaryExpression : ASTNode {
    TokenType operator_;
    std::unique_ptr<ASTNode> operand;
    
    UnaryExpression() { type = NodeType::UNARY_EXPRESSION; }
};