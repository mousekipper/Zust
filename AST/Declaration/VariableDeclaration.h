#ifndef VARIABLE_DECLARATION_H
#define VARIABLE_DECLARATION_H
#include <string>
#include <memory>
#include "../ASTNode.h"

struct VariableDeclaration : ASTNode {
    std::string dataType;
    std::string name;
    std::unique_ptr<ASTNode> initializer;
    
    VariableDeclaration() { type = NodeType::VARIABLE_DECLARATION; }
};

#endif // VARIABLE_DECLARATION_H