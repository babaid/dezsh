#include "AST.hpp"

#include <iomanip>

#include <print>

AST::AST(const std::vector<Token> &tokens)
{
    root_ = parseTokens(tokens);
}

std::unique_ptr<ASTNode> AST::parseTokens(const std::vector<Token> &tokens)
{
    if (tokens.empty())
        return nullptr;
    auto CmdToken = tokens[0];
    if (CmdToken.Type != TokenType::TOKEN_WORD)
        throw std::runtime_error("Parse error: expected command name");
    std::unique_ptr<CommandNode> root = std::make_unique<CommandNode>(CmdToken.Value);
    for (size_t i = 1; i < tokens.size(); ++i)
    {
        auto &token = tokens[i];
        if (token.Type == TokenType::TOKEN_WORD)

            root->addArgument(std::make_unique<LiteralNode>(token.Value));

        else if (token.Type == TokenType::TOKEN_EOF || token.Type == TokenType::TOKEN_SEMI)
            break;
    }
    return root;
}
void CommandNode::addArgument(std::unique_ptr<ArgumentNode> arg)
{
    args.push_back(std::move(arg));
}

int CommandNode::execute(ShellContext &context, std::istream& in, std::ostream& out)
{
    std::vector<std::string> evaluatedArguments;
    for (auto &argnode : args)
    {
        evaluatedArguments.push_back(argnode->evaluate(context));
    }
    auto it = context.commandRegistry.find(cmd);

    if (it != context.commandRegistry.end())
        return it->second->execute(context, evaluatedArguments, in, out);
    else
        return context.executeExternalCommand(cmd, evaluatedArguments, in, out);
}

std::string LiteralNode::evaluate(ShellContext &context)
{
    std::string result = "";
    std::string varname = "";
    size_t i = 0;

    while (i < value_.size())
    {
        if (value_[i] == '$')
        {
            ++i;
            if (value_[i] == '{')
            {
                ++i;
                while (value_[i] != '}')
                {
                    varname += value_[i];
                    ++i;
                }
                ++i;
            }
            else
            {
                while (i < value_.size() && (std::isalnum(value_[i]) || value_[i] == '_'))
                {
                    varname += value_[i];
                    ++i;
                }
            }
            auto envLookup = context.env.find(varname);
            if (envLookup != context.env.end())
            {
                result += envLookup->second;
            }
            else
            {
                result += "";
            }
            varname.clear();
        }
        else
        {
            result += value_[i];
            ++i;
        }
    }
    return result;
}
