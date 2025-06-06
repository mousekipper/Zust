#include "ASTNode.h"
#include "NodeType.h"
#include <string>
struct Identifier : ASTNode {
    std::string name;
    Identifier(const std::string& n) : name(n) { type = NodeType::IDENTIFIER; }
};