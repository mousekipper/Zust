#ifndef COMPILER_H
#define COMPILER_H
#include <string>
#include <sstream>
#include <fstream>
#include "Lexer.h"
#include "Parser.h"
#include "SemanticAnalyzer.h"
#include "CodeGenerator.h"
// ===== 메인 컴파일러 클래스 =====
class Compiler {
public:
    std::string compile(const std::string& sourceCode) {
        try {
            // 1. 렉싱
            Lexer lexer(sourceCode);
            auto tokens = lexer.tokenize();
            
            // 2. 파싱
            Parser parser(tokens);
            auto ast = parser.parse();
            
            // 3. 의미 분석
            SemanticAnalyzer analyzer;
            analyzer.analyze(ast.get());
            
            // 4. 코드 생성
            CodeGenerator generator;
            return generator.generate(ast.get());
            
        } catch (const std::exception& e) {
            return "Compilation Error: " + std::string(e.what());
        }
    }
    
    void compileFile(const std::string& inputFile, const std::string& outputFile) {
        std::ifstream inFile(inputFile);
        if (!inFile) {
            throw std::runtime_error("Cannot open input file: " + inputFile);
        }
        
        std::string sourceCode((std::istreambuf_iterator<char>(inFile)),
                               std::istreambuf_iterator<char>());
        
        std::string result = compile(sourceCode);
        
        std::ofstream outFile(outputFile);
        if (!outFile) {
            throw std::runtime_error("Cannot create output file: " + outputFile);
        }
        
        outFile << result;
    }
};
#endif // COMPILER_H