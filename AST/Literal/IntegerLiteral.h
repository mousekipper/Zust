#ifndef INTEGER_LITERAL_H
#define INTEGER_LITERAL_H
#include "../ASTNode.h"
struct IntegerLiteral : ASTNode {
    int value;
    IntegerLiteral(int v) : value(v) { type = NodeType::INTEGER_LITERAL; }
};
#endif // INTEGER_LITERAL_H