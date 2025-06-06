#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H
#include <vector>
#include <unordered_map>
#include <string>
#include "Symbol.h"
#include <stdexcept>

class SymbolTable {
private:
    std::vector<std::unordered_map<std::string, Symbol>> scopes;
    
public:
    SymbolTable() {
        pushScope(); // 전역 스코프
    }
    
    void pushScope() {
        scopes.emplace_back();
    }
    
    void popScope() {
        if (scopes.size() > 1) {
            scopes.pop_back();
        }
    }
    
    void declare(const Symbol& symbol) {
        if (scopes.back().find(symbol.name) != scopes.back().end()) {
            throw std::runtime_error("Variable '" + symbol.name + "' already declared in this scope");
        }
        scopes.back()[symbol.name] = symbol;
    }
    
    Symbol* lookup(const std::string& name) {
        for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
            auto found = it->find(name);
            if (found != it->end()) {
                return &found->second;
            }
        }
        return nullptr;
    }
};
#endif // SYMBOLTABLE_H