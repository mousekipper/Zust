#ifndef Symbol_h
#define Symbol_h

#include <string>
#include <unordered_map>
#include <vector>
#include <stdexcept>

// ===== 심볼 테이블 및 타입 검사 =====
struct Symbol {
    std::string name;
    std::string type;
    bool isFunction;
    std::vector<std::string> paramTypes;

    inline Symbol() : name(""), type(""), isFunction(0), paramTypes() {}
    inline Symbol(const std::string& n, const std::string& t, bool func = false) 
        : name(n), type(t), isFunction(func) {}
};

class SymbolTable {
private:
    std::vector<std::unordered_map<std::string, Symbol>> scopes;
    
public:
    inline SymbolTable() {
        pushScope(); // 전역 스코프
    }
    
    inline void pushScope() {
        scopes.emplace_back();
    }
    
    inline void popScope() {
        if (scopes.size() > 1) {
            scopes.pop_back();
        }
    }
    
    inline void declare(const Symbol& symbol) {
        if (scopes.back().find(symbol.name) != scopes.back().end()) {
            throw std::runtime_error("Variable '" + symbol.name + "' already declared in this scope");
        }
        scopes.back()[symbol.name] = symbol;
    }
    
    inline Symbol* lookup(const std::string& name) {
        for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
            auto found = it->find(name);
            if (found != it->end()) {
                return &found->second;
            }
        }
        return nullptr;
    }

    const inline Symbol* lookup(const std::string& name) const {
        for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
            auto found = it->find(name);
            if (found != it->end()) {
                return &found->second;
            }
        }
        return nullptr;
    }
};

#endif