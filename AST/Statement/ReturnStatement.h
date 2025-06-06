struct ReturnStatement : ASTNode {
    std::unique_ptr<ASTNode> expression;
    ReturnStatement() { type = NodeType::RETURN_STATEMENT; }
};