#include "CodeGenerator.h"

    void CodeGenerator::generateStatement(ASTNode* node){
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