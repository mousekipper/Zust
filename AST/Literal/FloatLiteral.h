struct FloatLiteral : ASTNode {
    double value;
    FloatLiteral(double v) : value(v) { type = NodeType::FLOAT_LITERAL; }
};