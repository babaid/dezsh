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

    for (size_t i = 0; i < tokens.size(); ++i)
    {
        auto &token = tokens[i];
        if (token.Type == TokenType::TOKEN_PIPE)
        {
            auto pipe = std::make_unique<PipeNode>();
            std::vector<Token> left(tokens.begin(), tokens.begin() + i);
            std::vector<Token> right(tokens.begin() + i + 1, tokens.end());
            if (!left.empty())
            pipe->left = parseTokens(left);
            if (!right.empty()) pipe->right = parseTokens(right);
            return pipe;
        }
        else if (token.Type == TokenType::TOKEN_EOF || token.Type == TokenType::TOKEN_SEMI)
            break;
    }

    auto cmd  = std::make_unique<CommandNode>(tokens[0].Value);
    
    for (size_t i = 1; i < tokens.size(); ++i)
    {
        auto& token = tokens[i];
        if (token.Type == TokenType::TOKEN_WORD)
        {
            cmd->addArgument(std::make_unique<LiteralNode>(token.Value));
        }
        else if (token.Type == TokenType::TOKEN_PIPE || token.Type == TokenType::TOKEN_SEMI || token.Type == TokenType::TOKEN_EOF) break;
    }
    return cmd;
}
void CommandNode::addArgument(std::unique_ptr<ArgumentNode> arg)
{
    args.push_back(std::move(arg));
}

int CommandNode::execute(ShellContext &context, std::istream &in, std::ostream &out)
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

#include <sstream>


int PipeNode::execute(ShellContext &context, std::istream &in, std::ostream &out)
{

    std::ostringstream temp;
    int RetVal = 0;
    if (left)
        left->execute(context, in, temp);
    std::istringstream temp2(temp.str());
    if (right)
    
        RetVal = right->execute(context, temp2, out);
    
    return RetVal;
}