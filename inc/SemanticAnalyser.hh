#ifndef SemanticAnalyser_hh
#define SemanticAnalyser_hh

#include "./Symbol.hh"


class Program;
class ASTNode;

#define ccfn

// ===== 의미 분석기 =====
class SemanticAnalyser {
private:
    SymbolTable symbolTable;
    std::string ccfn analyzeExpression(ASTNode* node);
    void ccfn analyzeStatement(ASTNode* node, std::string* expectedType = nullptr);

public:
    void ccfn analyze(Program* program);
};

#endif