#ifndef LEXER_HPP
#define LEXER_HPP

#include <string>
#include <vector>

enum TokenType {
  TOKEN_WORD,
  VARIABLE,
  TOKEN_PIPE,
  TOKEN_REDIRECT_OUT,
  TOKEN_REDIRECT_OUT_APPEND,
  TOKEN_REDIRECT_IN,
  TOKEN_AND_IF,
  TOKEN_OR_IF,
  TOKEN_SEMI,
  TOKEN_BACKGROUND,
  TOKEN_LPAREN,
  TOKEN_RPAREN,
  TOKEN_EOF
};

struct Token {
  TokenType Type;
  std::string Value;
  size_t line, column;
};

struct LexerState {
  std::string input;
  char curr;
  size_t pos, line, col;

  LexerState(const std::string &input)
      : input(input), curr('\0'), pos(0), line(0), col(0) {
    if (!input.empty())
      curr = input[0];
  }
};

class Lexer {
public:
  Lexer(const std::string &input);
  std::vector<Token> tokenize();

private:
  // represents the current state
  LexerState state_;

  Token nextToken();
  char peek();
  void advance();
  void skipWhiteSpace();
  Token parseWord();
  Token parseOperator();
  Token parseQuotedString();
};

#endif
