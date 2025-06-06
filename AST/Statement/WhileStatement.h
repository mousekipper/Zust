struct WhileStatement : ASTNode {
    std::unique_ptr<ASTNode> condition;
    std::unique_ptr<ASTNode> body;
    
    WhileStatement() { type = NodeType::WHILE_STATEMENT; }
};