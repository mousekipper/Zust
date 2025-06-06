struct StringLiteral : ASTNode {
    std::string value;
    StringLiteral(const std::string& v) : value(v) { type = NodeType::STRING_LITERAL; }
};