#ifndef WHILE_STATEMENT_H
#define WHILE_STATEMENT_H
struct WhileStatement : ASTNode {
    std::unique_ptr<ASTNode> condition;
    std::unique_ptr<ASTNode> body;
    
    WhileStatement() { type = NodeType::WHILE_STATEMENT; }
};
#endif // WHILE_STATEMENT_H