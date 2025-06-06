#ifndef AST_EXPRESSION_STATEMENT_H
#define AST_EXPRESSION_STATEMENT_H
struct ExpressionStatement : ASTNode {
    std::unique_ptr<ASTNode> expression;
    ExpressionStatement() { type = NodeType::EXPRESSION_STATEMENT; }
};
#endif // AST_EXPRESSION_STATEMENT_H