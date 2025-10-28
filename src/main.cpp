#include "Lexer.hpp"
#include "AST.hpp"
#include <iostream>

int main() {
  while (true) {
    std::string line;
    std:: cout << ":>";
    std::getline(std::cin, line);
    if (line.empty()) continue;
    Lexer Lexer(line);
    auto tokens = Lexer.tokenize();
    ShellContext context;
    AST ast(tokens);
    ast.execute(context);
  }
}
