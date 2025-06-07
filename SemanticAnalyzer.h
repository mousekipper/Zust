#ifndef SEMANTIC_ANALYZER_H
#define SEMANTIC_ANALYZER_H
#include "SymbolTable.h"
#include "AST/AST.h"
#include "Program.h"

// ===== 의미 분석기 =====
class SemanticAnalyzer {
private:
    SymbolTable symbolTable;
    
    std::string analyzeExpression(ASTNode* node) {
        if (!node) return "void";
        
        switch (node->type) {
            case NodeType::INTEGER_LITERAL:
                return "int";
            case NodeType::FLOAT_LITERAL:
                return "float";
            case NodeType::STRING_LITERAL:
                return "string";
            case NodeType::BOOL_LITERAL:
                return "bool";
            case NodeType::IDENTIFIER: {
                auto id = static_cast<Identifier*>(node);
                Symbol* symbol = symbolTable.lookup(id->name);
                if (!symbol) {
                    throw std::runtime_error("Undefined variable: " + id->name);
                }
                return symbol->type;
            }
            case NodeType::BINARY_EXPRESSION: {
                auto binary = static_cast<BinaryExpression*>(node);
                std::string leftType = analyzeExpression(binary->left.get());
                std::string rightType = analyzeExpression(binary->right.get());
                
                // 타입 호환성 검사
                if (leftType != rightType) {
                    throw std::runtime_error("Type mismatch in binary expression");
                }

                switch (binary->operator_){
                    case TokenType::EQUAL:
                    case TokenType::NOT_EQUAL:
                    case TokenType::LESS:
                    case TokenType::LESS_EQUAL:
                    case TokenType::GREATER:
                    case TokenType::GREATER_EQUAL:
                        return "bool"; // 비교 연산자는 항상 bool 반환
                    
                }
                
                return leftType;
            }
            case NodeType::ASSIGNMENT_EXPRESSION: {
                auto assignment = static_cast<AssignmentExpression*>(node);
                std::string leftType = analyzeExpression(assignment->left.get());
                std::string rightType = analyzeExpression(assignment->right.get());
                
                if (leftType != rightType) {
                    throw std::runtime_error("Type mismatch in assignment");
                }
                
                return leftType;
            }
            case NodeType::CALL_EXPRESSION: {
                auto call = static_cast<CallExpression*>(node);
                if (call->callee->type == NodeType::IDENTIFIER) {
                    auto id = static_cast<Identifier*>(call->callee.get());
                    Symbol* symbol = symbolTable.lookup(id->name);
                    if (!symbol || !symbol->isFunction) {
                        throw std::runtime_error("Undefined function: " + id->name);
                    }
                    
                    // 매개변수 타입 검사
                    if (call->arguments.size() != symbol->paramTypes.size()) {
                        throw std::runtime_error("Argument count mismatch for function: " + id->name);
                    }
                    
                    for (size_t i = 0; i < call->arguments.size(); ++i) {
                        std::string argType = analyzeExpression(call->arguments[i].get());
                        if (argType != symbol->paramTypes[i]) {
                            throw std::runtime_error("Argument type mismatch for function: " + id->name);
                        }
                    }
                    
                    return symbol->type;
                }
                break;
            }
            default:
                break;
        }
        
        return "void";
    }
    
    void analyzeStatement(ASTNode* node, std::string* expectedType = nullptr) {
        if (!node) return;
        switch (node->type) {
            case NodeType::VARIABLE_DECLARATION: {
                auto var = static_cast<VariableDeclaration*>(node);
                
                if (var->initializer) {
                    std::string initType = analyzeExpression(var->initializer.get());
                    if (!var->dataType.empty() && var->dataType != initType) {
                        throw std::runtime_error("Type mismatch in variable declaration: " + var->name);
                    }
                    if (var->dataType.empty()) {
                        var->dataType = initType; // 타입 추론
                    }
                }
                
                symbolTable.declare(Symbol(var->name, var->dataType));
                break;
            }
            case NodeType::FUNCTION_DECLARATION: {
                auto func = static_cast<FunctionDeclaration*>(node);
                
                Symbol funcSymbol(func->name, func->returnType, true);
                for (const auto& param : func->parameters) {
                    funcSymbol.paramTypes.push_back(param.first);
                }
                
                symbolTable.declare(funcSymbol);
                
                // 함수 본문 분석
                symbolTable.pushScope();
                
                // 매개변수를 스코프에 추가
                for (const auto& param : func->parameters) {
                    symbolTable.declare(Symbol(param.second, param.first));
                }
                
                if (func->body) {
                    analyzeStatement(func->body.get(), &(func->returnType));
                }
                
                symbolTable.popScope();
                break;
            }
            case NodeType::BLOCK_STATEMENT: {
                auto block = static_cast<BlockStatement*>(node);
                symbolTable.pushScope();
                
                for (const auto& stmt : block->statements) {
                    analyzeStatement(stmt.get(), expectedType);
                }
                
                symbolTable.popScope();
                break;
            }
            case NodeType::IF_STATEMENT: {
                auto ifStmt = static_cast<IfStatement*>(node);
                
                std::string condType = analyzeExpression(ifStmt->condition.get());
                if (condType != "bool") {
                    throw std::runtime_error("If condition must be boolean");
                }
                
                analyzeStatement(ifStmt->thenStatement.get());
                if (ifStmt->elseStatement) {
                    analyzeStatement(ifStmt->elseStatement.get());
                }
                break;
            }
            case NodeType::WHILE_STATEMENT: {
                auto whileStmt = static_cast<WhileStatement*>(node);
                
                std::string condType = analyzeExpression(whileStmt->condition.get());
                if (condType != "bool") {
                    throw std::runtime_error("While condition must be boolean");
                }
                
                analyzeStatement(whileStmt->body.get());
                break;
            }
            case NodeType::RETURN_STATEMENT: {
                auto returnStmt = static_cast<ReturnStatement*>(node);
                if (returnStmt->expression) {
                    std::string returnType = analyzeExpression(returnStmt->expression.get());
                    if (*expectedType == "auto"){
                        *expectedType = returnType; // auto 타입 추론
                    }
                    if (expectedType->compare(returnType)!=0){
                        throw std::runtime_error("Return type mismatch: expected " + *expectedType + ", got " + returnType);
                    }
                }
                break;
            }
            case NodeType::EXPRESSION_STATEMENT: {
                auto exprStmt = static_cast<ExpressionStatement*>(node);
                analyzeExpression(exprStmt->expression.get());
                break;
            }
            case NodeType::NAMESPACE_DECLARATION:{
                NamespaceDeclaration* ns = static_cast<NamespaceDeclaration*>(node);
                symbolTable.pushScope();
                if (ns->body) {
                    analyzeStatement(ns->body.get());
                    
                }
                symbolTable.popScope();
                break;
            }
            default:
                break;
        }
    }
    
public:
    void analyze(Program* program) {
        for (const auto& stmt : program->statements) {
            analyzeStatement(stmt.get());
        }
    }
};
#endif // SEMANTIC_ANALYZER_H