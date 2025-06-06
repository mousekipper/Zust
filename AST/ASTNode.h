#ifndef ASTNODE_H
#define ASTNODE_H
#include "NodeType.h"

struct ASTNode {
    NodeType type;
    virtual ~ASTNode() = default;
};
#endif // ASTNODE_H