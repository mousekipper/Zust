#ifndef FLOAT_LITERAL_H
#define FLOAT_LITERAL_H
struct FloatLiteral : ASTNode {
    double value;
    FloatLiteral(double v) : value(v) { type = NodeType::FLOAT_LITERAL; }
};
#endif // FLOAT_LITERAL_H