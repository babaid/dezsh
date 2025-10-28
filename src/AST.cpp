#include "AST.hpp"
#include <iomanip>

#include<iostream>
AST::AST(const std::vector<Token>&  tokens)
{
    root_ = parseTokens(tokens);
}

std::unique_ptr<ASTNode> AST::parseTokens(const std::vector<Token>&tokens)
{
    if (tokens.empty()) return nullptr;
    auto CmdToken = tokens[0];
    if (CmdToken.Type != TokenType::TOKEN_WORD) throw std::runtime_error("Parse error: expected command name");
    std::unique_ptr<CommandNode> root = std::make_unique<CommandNode>(CmdToken.Value);
    for (size_t i=1; i<tokens.size(); ++i)
    {
        auto &token = tokens[i];
        if (token.Type == TokenType::TOKEN_WORD)
        
            root->addArgument(std::make_unique<LiteralNode>(token.Value));
        
        else if (token.Type == TokenType::TOKEN_EOF || token.Type == TokenType::TOKEN_SEMI) break;
    }
    return root;

}
void CommandNode::addArgument(std::unique_ptr<ArgumentNode> arg)
{
    args.push_back(std::move(arg));
}

int CommandNode::execute(ShellContext &context) {
    std::vector<std::string> evaluatedArguments;
    for(auto& argnode:args)
    
    {
        evaluatedArguments.push_back(argnode->evaluate(context));
    }
    auto it = context.commandRegistry.find(cmd);

    if (it != context.commandRegistry.end()) return it->second(context, evaluatedArguments);
    else return context.executeExternalCommand(cmd, evaluatedArguments);
}

