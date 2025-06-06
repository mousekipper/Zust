#ifndef AST_BLOCK_STATEMENT_H
#define AST_BLOCK_STATEMENT_H

#include "../ASTNode.h"
#include <vector>
#include <memory>
#include "../NodeType.h"
struct BlockStatement : ASTNode {
    std::vector<std::unique_ptr<ASTNode>> statements;
    BlockStatement() { type = NodeType::BLOCK_STATEMENT; }
};
#endif // AST_BLOCK_STATEMENT_H