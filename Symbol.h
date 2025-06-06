#ifndef SYMBOL_H
#define SYMBOL_H
#include <string>
#include <vector>

// ===== 심볼 테이블 및 타입 검사 =====
struct Symbol {
    std::string name;
    std::string type;
    bool isFunction;
    std::vector<std::string> paramTypes;
    
    Symbol() : isFunction(false) {}

    Symbol(const std::string& n, const std::string& t, bool func = false) 
        : name(n), type(t), isFunction(func) {}
};
#endif // SYMBOL_H