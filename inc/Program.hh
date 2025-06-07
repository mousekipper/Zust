#ifndef Program_hh
#define Program_hh

#include "./ASTNode.hh"
#include <vector>
#include <memory>

struct Program : ASTNode {
    std::vector<std::unique_ptr<ASTNode>> statements;
    inline Program() : ASTNode(NodeType::PROGRAM) {  }
};

#endif