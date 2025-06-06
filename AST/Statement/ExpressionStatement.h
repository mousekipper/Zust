struct ExpressionStatement : ASTNode {
    std::unique_ptr<ASTNode> expression;
    ExpressionStatement() { type = NodeType::EXPRESSION_STATEMENT; }
};