#ifndef Nodes_hh
#define  Nodes_hh

#include <string>
#include "./ASTNode.hh"
#include <memory>
#include <vector>

template<NodeType a>
struct NodeVAR : public ASTNode {
    constexpr static NodeType var = a;
    inline NodeVAR() : ASTNode(a) {}
};
template<NodeType a>
struct Node;
#define NodeDef(a) template<> struct Node<a> : public NodeVAR<a>
#define NodeConstruct(...) Node(__VA_ARGS__) : NodeVAR()


NodeDef(NodeType::VARIABLE_DECLARATION) {
    std::string dataType;
    std::string name;
    std::unique_ptr<ASTNode> initializer;
    inline NodeConstruct() {}
};


NodeDef(NodeType::FUNCTION_DECLARATION) {
    std::string returnType;
    std::string name;
    std::vector<std::pair<std::string, std::string>> parameters; // type, name
    std::unique_ptr<ASTNode> body;
    inline NodeConstruct() {}
};

NodeDef(NodeType::BLOCK_STATEMENT) {
    std::vector<std::unique_ptr<ASTNode>> statements;
    inline NodeConstruct() {}
};

NodeDef(NodeType::IF_STATEMENT) {
    std::unique_ptr<ASTNode> condition;
    std::unique_ptr<ASTNode> thenStatement;
    std::unique_ptr<ASTNode> elseStatement;
    inline NodeConstruct() {}
};

NodeDef(NodeType::WHILE_STATEMENT) {
    std::unique_ptr<ASTNode> condition;
    std::unique_ptr<ASTNode> body;
    inline NodeConstruct() {}
};

NodeDef(NodeType::RETURN_STATEMENT) {
    std::unique_ptr<ASTNode> expression;
};

NodeDef(NodeType::EXPRESSION_STATEMENT) {
    std::unique_ptr<ASTNode> expression;
    inline NodeConstruct() {}
    inline ~Node() {}
};


#include "./TokenType.hh"

NodeDef(NodeType::BINARY_EXPRESSION) {
    std::unique_ptr<ASTNode> left;
    TokenType operator_;
    std::unique_ptr<ASTNode> right;
    inline NodeConstruct(), operator_() {}
};

NodeDef(NodeType::UNARY_EXPRESSION) {
    TokenType operator_;
    std::unique_ptr<ASTNode> operand;
    inline NodeConstruct(), operator_() {}
};

NodeDef(NodeType::CALL_EXPRESSION) {
    std::unique_ptr<ASTNode> callee;
    std::vector<std::unique_ptr<ASTNode>> arguments;
    inline NodeConstruct() {}
};

NodeDef(NodeType::IDENTIFIER) {
    std::string name;
    inline NodeConstruct(const std::string& a), name(a) {}
};

NodeDef(NodeType::INTEGER_LITERAL) {
    int value;
    inline NodeConstruct(int v) , value(v) {}
};

NodeDef(NodeType::FLOAT_LITERAL) {
    double value;
    inline NodeConstruct(double v) , value(v) {}
};

NodeDef(NodeType::STRING_LITERAL) {
    std::string value;
    inline NodeConstruct(const std::string& v) , value(v) {}
    inline NodeConstruct(const char* v) , value(v) {}
};

NodeDef(NodeType::BOOL_LITERAL) {
    bool value;
    inline NodeConstruct(bool v), value(v) {}
};

NodeDef(NodeType::ASSIGNMENT_EXPRESSION) {
    std::unique_ptr<ASTNode> left;
    std::unique_ptr<ASTNode> right;
    inline NodeConstruct() {}
};

NodeDef(NodeType::NAMESPACE_DECLARATION) {
    std::string name;
    std::unique_ptr<ASTNode> body;
    inline NodeConstruct() {}
};

#endif