#include <exception>
#include <string>

// ===== 에러 처리 =====
class CompilerError : public std::exception {
public:
    enum Type { LEXICAL, SYNTAX, SEMANTIC, RUNTIME };
    
private:
    Type errorType;
    std::string message;
    int line;
    int column;
    
public:
    CompilerError(Type type, const std::string& msg, int l = 0, int c = 0)
        : errorType(type), message(msg), line(l), column(c) {}
    
    const char* what() const noexcept override {
        static std::string fullMessage;
        fullMessage = getTypeString() + " Error";
        if (line > 0) {
            fullMessage += " at line " + std::to_string(line);
            if (column > 0) {
                fullMessage += ", column " + std::to_string(column);
            }
        }
        fullMessage += ": " + message;
        return fullMessage.c_str();
    }
    
    std::string getTypeString() const {
        switch (errorType) {
            case LEXICAL: return "Lexical";
            case SYNTAX: return "Syntax";
            case SEMANTIC: return "Semantic";
            case RUNTIME: return "Runtime";
            default: return "Unknown";
        }
    }
};
