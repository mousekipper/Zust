#ifndef PROGRAM_H
#define PROGRAM_H
#include <vector>
#include <memory>
#include "AST/ASTNode.h"

struct Program : ASTNode {
    std::vector<std::unique_ptr<ASTNode>> statements;
    Program() { type = NodeType::PROGRAM; }
};
#endif // PROGRAM_H