#include "Lexer.hpp"
#include "AST.hpp"
#include <iostream>

int main() {
  ShellContext context;
  while (context.running) {
    std::string line;
    std:: cout << ":>";
    std::getline(std::cin, line);
    if (line.empty()) continue;
    Lexer Lexer(line);
    auto tokens = Lexer.tokenize();
    
    AST ast(tokens);
    ast.execute(context);
  }
  return context.exit_code;
}

