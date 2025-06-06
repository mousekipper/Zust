#ifndef RETURN_STATEMENT_H
#define RETURN_STATEMENT_H
struct ReturnStatement : ASTNode {
    std::unique_ptr<ASTNode> expression;
    ReturnStatement() { type = NodeType::RETURN_STATEMENT; }
};
#endif // RETURN_STATEMENT_H