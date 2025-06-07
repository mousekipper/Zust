#ifndef ASTNode_hh
#define ASTNode_hh

#include "./NodeType.hh"

struct ASTNode {
    NodeType type;
    // virtual ~ASTNode() = default;
    constexpr ASTNode(NodeType type) : type(type) {}
};

#endif