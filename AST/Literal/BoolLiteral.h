

struct BoolLiteral : ASTNode {
    bool value;
    BoolLiteral(bool v) : value(v) { type = NodeType::BOOL_LITERAL; }
};
