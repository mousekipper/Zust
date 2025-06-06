struct FunctionDeclaration : ASTNode {
    std::string returnType;
    std::string name;
    std::vector<std::pair<std::string, std::string>> parameters; // type, name
    std::unique_ptr<ASTNode> body;
    
    FunctionDeclaration() { type = NodeType::FUNCTION_DECLARATION; }
};