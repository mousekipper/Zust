#ifndef BOOL_LITERAL_H
#define BOOL_LITERAL_H

struct BoolLiteral : ASTNode {
    bool value;
    BoolLiteral(bool v) : value(v) { type = NodeType::BOOL_LITERAL; }
};
#endif // BOOL_LITERAL_H
