#ifndef NAMESPACE_DECLARATION_H
#define NAMESPACE_DECLARATION_H
struct NamespaceDeclaration : ASTNode {
    std::string name;
    std::unique_ptr<ASTNode> body;
    
    NamespaceDeclaration() { type = NodeType::NAMESPACE_DECLARATION; }
};

#endif // NAMESPACE_DECLARATION_H