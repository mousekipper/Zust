#include <Compiler.hh>
#include <iostream>
#include <memory>

// ===== 메인 함수 및 테스트 =====
int main(int argc, char* argv[]) {
    try {
        Compiler compiler;
        
        if (argc == 3) {
            // 파일 컴파일 모드
            compiler.compileFile(argv[1], argv[2]);
            std::cout << "Compilation successful: " << argv[1] << " -> " << argv[2] << std::endl;
        } else {
            // 테스트 모드
            std::string testCode = R"(
# 테스트 프로그램
namespace TestNamespace {
    fn add(int a, int b) {
        return a + b;
    }
    
    fn main() {
        let x: int = 10;
        let y: int = 20;
        let result: int = add(x, y);
        
        if (result > 25) {
            let message: string = "Result is greater than 25";
        }
        
        let i: int = 0;
        while (i < 5) {
            i = i + 1;
        }
        
        return 0;
    }
}
)";
            
            std::string result = compiler.compile(testCode);
            std::cout << "Generated C++ Code:\n" << std::endl;
            std::cout << result << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
