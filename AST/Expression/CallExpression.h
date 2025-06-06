#include "../ASTNode.h"
#include <memory>
#include <vector>

struct CallExpression : ASTNode {
    std::unique_ptr<ASTNode> callee;
    std::vector<std::unique_ptr<ASTNode>> arguments;
    
    CallExpression() { type = NodeType::CALL_EXPRESSION; }
};
