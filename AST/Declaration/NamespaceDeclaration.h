struct NamespaceDeclaration : ASTNode {
    std::string name;
    std::unique_ptr<ASTNode> body;
    
    NamespaceDeclaration() { type = NodeType::NAMESPACE_DECLARATION; }
};
