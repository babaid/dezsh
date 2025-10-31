#ifndef AST_H
#define AST_H

#include "Lexer.hpp"
#include "ShellContext.hpp"

#include <functional>
#include <unordered_map>
#include<memory>
#include<stdexcept>

class ArgumentNode;

class ASTNode {
public:
virtual ~ASTNode() = default;
  virtual int execute(ShellContext &context, std::istream& in, std::ostream& out) = 0;
  virtual std::string toString() = 0;
  virtual TokenType getType() = 0;
};

class AST{
public:
AST(const std::vector<Token>&);
int execute(ShellContext& context, std::istream& in, std::ostream& out){if (root_) return root_->execute(context, in, out); else return 1;};
private:
std::unique_ptr<ASTNode> parseTokens(const std::vector<Token>&);
std::unique_ptr<ASTNode> root_;
};

class ArgumentNode: public ASTNode
{
  public:
  virtual std::string evaluate(ShellContext& context) = 0;
  virtual std::string toString() override = 0;
  virtual TokenType getType() override =0;
};

class LiteralNode: public ArgumentNode
{
  public:
  LiteralNode(std::string value): value_(std::move(value)){}
  int execute(ShellContext &context, std::istream& in, std::ostream& out) override {
    throw std::runtime_error("LiteralNode cannot be executed");
} 
  std::string evaluate(ShellContext& context) override;
  std::string toString() override { return "LiteralNode("+value_+")"; }
  TokenType getType() override { return TokenType::TOKEN_WORD; }
  private:
  std::string value_;
};

class CommandNode : public ASTNode {
public:
  CommandNode(std::string CmdName): cmd(CmdName){};
  void addArgument(std::unique_ptr<ArgumentNode> arg);
  int execute(ShellContext &context, std::istream& in, std::ostream& out) override;
  std::string toString() override { return "CommandNode"; }
  TokenType getType() override { return TokenType::TOKEN_WORD; }

private:
  std::string cmd;
  std::vector<std::unique_ptr<ArgumentNode>> args;
};

class PipeNode : public ASTNode {
public:
 
  int execute(ShellContext &context, std::istream& in, std::ostream& out) override;
  std::string toString() override { return "PipeNode"; }
  TokenType getType() override { return TokenType::TOKEN_PIPE; }
 std::unique_ptr<ASTNode> left;
 std::unique_ptr<ASTNode> right;
};


#endif
