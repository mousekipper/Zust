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
    
    void generateExpression(ASTNode* node) {
        if (!node) return;
        
        switch (node->type) {
            case NodeType::INTEGER_LITERAL: {
                auto lit = static_cast<IntegerLiteral*>(node);
                output << lit->value;
                break;
            }
            case NodeType::FLOAT_LITERAL: {
                auto lit = static_cast<FloatLiteral*>(node);
                output << lit->value;
                break;
            }
            case NodeType::STRING_LITERAL: {
                auto lit = static_cast<StringLiteral*>(node);
                output << "\"" << lit->value << "\"";
                break;
            }
            case NodeType::BOOL_LITERAL: {
                auto lit = static_cast<BoolLiteral*>(node);
                output << (lit->value ? "true" : "false");
                break;
            }
            case NodeType::IDENTIFIER: {
                auto id = static_cast<Identifier*>(node);
                output << id->name;
                break;
            }
            case NodeType::BINARY_EXPRESSION: {
                auto binary = static_cast<BinaryExpression*>(node);
                output << "(";
                generateExpression(binary->left.get());
                
                switch (binary->operator_) {
                    case TokenType::PLUS: output << " + "; break;
                    case TokenType::MINUS: output << " - "; break;
                    case TokenType::MULTIPLY: output << " * "; break;
                    case TokenType::DIVIDE: output << " / "; break;
                    case TokenType::MODULO: output << " % "; break;
                    case TokenType::EQUAL: output << " == "; break;
                    case TokenType::NOT_EQUAL: output << " != "; break;
                    case TokenType::LESS: output << " < "; break;
                    case TokenType::GREATER: output << " > "; break;
                    case TokenType::LESS_EQUAL: output << " <= "; break;
                    case TokenType::GREATER_EQUAL: output << " >= "; break;
                    case TokenType::LOGICAL_AND: output << " && "; break;
                    case TokenType::LOGICAL_OR: output << " || "; break;
                    case TokenType::BIT_AND: output << " & "; break;
                    case TokenType::BIT_OR: output << " | "; break;
                    case TokenType::BIT_XOR: output << " ^ "; break;
                    case TokenType::LEFT_SHIFT: output << " << "; break;
                    case TokenType::RIGHT_SHIFT: output << " >> "; break;
                    default: output << " OP "; break;
                }
                
                generateExpression(binary->right.get());
                output << ")";
                break;
            }
            case NodeType::ASSIGNMENT_EXPRESSION: {
                auto assignment = static_cast<AssignmentExpression*>(node);
                generateExpression(assignment->left.get());
                output << " = ";
                generateExpression(assignment->right.get());
                break;
            }
            case NodeType::CALL_EXPRESSION: {
                auto call = static_cast<CallExpression*>(node);
                generateExpression(call->callee.get());
                output << "(";
                
                for (size_t i = 0; i < call->arguments.size(); ++i) {
                    if (i > 0) output << ", ";
                    generateExpression(call->arguments[i].get());
                }
                
                output << ")";
                break;
            }
            default:
                break;
        }
    }
    
    void generateStatement(ASTNode* node) {
        if (!node) return;
        
        switch (node->type) {
            case NodeType::VARIABLE_DECLARATION: {
                auto var = static_cast<VariableDeclaration*>(node);
                indent();
                
                // C++ 타입 매핑
                std::string cppType = mapToCppType(var->dataType);
                output << cppType << " " << var->name;
                
                if (var->initializer) {
                    output << " = ";
                    generateExpression(var->initializer.get());
                }
                
                output << ";\n";
                break;
            }
            case NodeType::FUNCTION_DECLARATION: {
                auto func = static_cast<FunctionDeclaration*>(node);
                indent();
                
                std::string returnType = mapToCppType(func->returnType);
                output << returnType << " " << func->name << "(";
                
                for (size_t i = 0; i < func->parameters.size(); ++i) {
                    if (i > 0) output << ", ";
                    output << mapToCppType(func->parameters[i].first) << " " << func->parameters[i].second;
                }
                
                output << ")";
                
                if (func->body) {
                    output << " ";
                    generateStatement(func->body.get());
                } else {
                    output << ";\n";
                }
                break;
            }
            case NodeType::BLOCK_STATEMENT: {
                auto block = static_cast<BlockStatement*>(node);
                output << "{\n";
                indentLevel++;
                
                for (const auto& stmt : block->statements) {
                    generateStatement(stmt.get());
                }
                
                indentLevel--;
                indent();
                output << "}\n";
                break;
            }
            case NodeType::IF_STATEMENT: {
                auto ifStmt = static_cast<IfStatement*>(node);
                indent();
                output << "if (";
                generateExpression(ifStmt->condition.get());
                output << ") ";
                
                generateStatement(ifStmt->thenStatement.get());
                
                if (ifStmt->elseStatement) {
                    indent();
                    output << "else ";
                    generateStatement(ifStmt->elseStatement.get());
                }
                break;
            }
            case NodeType::WHILE_STATEMENT: {
                auto whileStmt = static_cast<WhileStatement*>(node);
                indent();
                output << "while (";
                generateExpression(whileStmt->condition.get());
                output << ") ";
                
                generateStatement(whileStmt->body.get());
                break;
            }
            case NodeType::RETURN_STATEMENT: {
                auto returnStmt = static_cast<ReturnStatement*>(node);
                indent();
                output << "return";
                
                if (returnStmt->expression) {
                    output << " ";
                    generateExpression(returnStmt->expression.get());
                }
                
                output << ";\n";
                break;
            }
            case NodeType::EXPRESSION_STATEMENT: {
                auto exprStmt = static_cast<ExpressionStatement*>(node);
                indent();
                generateExpression(exprStmt->expression.get());
                output << ";\n";
                break;
            }
            case NodeType::NAMESPACE_DECLARATION: {
                auto ns = static_cast<NamespaceDeclaration*>(node);
                indent();
                output << "namespace " << ns->name << " ";
                generateStatement(ns->body.get());
                break;
            }
            default:
                break;
        }
    }
    
    std::string mapToCppType(const std::string& type) {
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