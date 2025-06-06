#include "../ASTNode.h"


struct IntegerLiteral : ASTNode {
    int value;
    IntegerLiteral(int v) : value(v) { type = NodeType::INTEGER_LITERAL; }
};