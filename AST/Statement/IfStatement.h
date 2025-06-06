#ifndef IFSTATEMENT_H
#define IFSTATEMENT_H
#include "../ASTNode.h"
#include <memory>
struct IfStatement : ASTNode {
    std::unique_ptr<ASTNode> condition;
    std::unique_ptr<ASTNode> thenStatement;
    std::unique_ptr<ASTNode> elseStatement;
    
    IfStatement() { type = NodeType::IF_STATEMENT; }
};
#endif // IFSTATEMENT_H