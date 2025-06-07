#include <sstream>
#include "AST/AST.h"

#include "Program.h"
// ===== 코드 생성기 =====
class CodeGenerator {
private:
    std::ostringstream output;
    int indentLevel = 0;
    
    void indent() {
        for (int i = 0; i < indentLevel; ++i) {
            output << "    ";
        }
    }
    
    void generateExpression(ASTNode* node) ;
    
    void generateStatement(ASTNode* node) ;
    
    std::string mapToCppType(const std::string& type){
        if (type == "int") return "int";
        if (type == "float") return "float";
        if (type == "double") return "double";
        if (type == "char") return "char";
        if (type == "bool") return "bool";
        if (type == "string") return "std::string";
        if (type == "byte") return "unsigned char";
        if (type == "short") return "short";
        if (type == "long") return "long";
        if (type == "void") return "void";
        return type;
    }
    
public:
    std::string generate(Program* program) {
        output << "#include <iostream>\n";
        output << "#include <string>\n";
        output << "#include <cmath>\n\n";
        
        for (const auto& stmt : program->statements) {
            generateStatement(stmt.get());
        }
        
        return output.str();
    }
};