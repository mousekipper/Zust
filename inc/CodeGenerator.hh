#ifndef CodeGenerator_hh
#define CodeGenerator_hh

#include <sstream>
class ASTNode;
class Program;

#define ccfn

// ===== 코드 생성기 =====
class CodeGenerator {
private:
    std::ostringstream output;
    int indentLevel = 0;
    
    inline void indent() {
        for (int i = 0; i < indentLevel; ++i) {
            output << "    ";
        }
    }
public:
    void ccfn generateExpression(ASTNode* node);
    void ccfn generateStatement(ASTNode* node);
    std::string ccfn mapToCppType(const std::string& type) const;
    std::string ccfn generate(Program* program);
};

#endif