#include <Parser.hh>

#define ccfn Parser::

bool ccfn match(TokenType type, bool skip) {
    if (current().type == type) {
        pos += skip;
        return true;
    }
    return false;
}

void ccfn expect(TokenType type) {
    if (!match(type)) {
        throw std::runtime_error(
            "Expected token type " 
            + std::to_string((int)type) +
            " but " 
            + std::to_string((int)(current().type)) 
            + " at line " 
            + std::to_string(current().line)
        );
    }
}

void ccfn skipNewlines() {
    while (match(TokenType::NEWLINE) || match(TokenType::COMMENT)) {}
}

std::unique_ptr<Program> ccfn parse() {
    auto program = std::make_unique<Program>();
    TokenType tktype;
    skipNewlines();
    
    while ((tktype = current().type) != TokenType::EOF_TOKEN) {
        switch(tktype) {
            case TokenType::NAMESPACE:
                program->statements.push_back(parseNamespaceDeclaration());
                break;
            case TokenType::IMPORT:
                program->statements.push_back(parseImportStatement());
                break;
            case TokenType::FN:
                program->statements.push_back(parseFunctionDeclaration());
                break;
            case TokenType::LET:
                program->statements.push_back(parseVariableDeclaration());
                break;
            default:
                program->statements.push_back(parseStatement());
                break;
        }
        skipNewlines();
    }
    
    return program;
}

