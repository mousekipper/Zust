#include <Compiler.hh>

#include <memory>
#include <vector>
#include <Program.hh>
#include <Token.hh>
#include <Lexer.hh>
#include <Parser.hh>
#include <SemanticAnalyser.hh>
#include <CodeGenerator.hh>
#include <fstream>

#undef ccfn
#define ccfn Compiler::

std::string ccfn compile(const std::string& sourceCode) {
    std::unique_ptr<Program> ast = 0;
    std::vector<Token> tokens;

    try {
        // 1. 렉싱
        Lexer lexer(sourceCode);
        tokens = lexer.tokenize();
        
    } catch (const std::exception& e) {
        return "Compilation Error: " + std::string(e.what());
    }
                // 2. 파싱
        Parser parser(tokens);
        ast = parser.parse();

        // 3. 의미 분석
        SemanticAnalyser analyzer;
        analyzer.analyze(ast.get());
        
        // 4. 코드 생성
        CodeGenerator generator;
        return generator.generate(ast.get());
}

void ccfn compileFile(const std::string& inputFile, const std::string& outputFile) {
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