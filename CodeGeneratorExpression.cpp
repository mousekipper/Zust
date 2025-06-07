#include "CodeGenerator.h"


void CodeGenerator::generateExpression(ASTNode* node){
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