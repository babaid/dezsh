#include "Lexer.hpp"
#include <cctype>

Lexer::Lexer(const std::string &input) : state_(input) {}

std::vector<Token> Lexer::tokenize()
{

  std::vector<Token> tokens;
  Token token;

  do
  {
    token = nextToken();
    tokens.push_back(token);
  } while (token.Type != TOKEN_EOF);
  return tokens;
}

void Lexer::advance()
{
  if (state_.curr == '\n')
  {
    state_.line++;
    state_.col = 0;
  }
  else
  {
    state_.col++;
  }

  state_.pos++;

  if (state_.pos >= state_.input.size())
  {
    state_.curr = '\0';
  }
  else
  {
    state_.curr = state_.input[state_.pos];
  }
}

char Lexer::peek()
{
  size_t nextPos = state_.pos + 1;
  if (nextPos >= state_.input.size())
    return '\0';
  else
    return state_.input[nextPos];
}

void Lexer::skipWhiteSpace()
{
  while (state_.curr != '\0' && std::isspace(state_.curr))
  {
    this->advance();
  }
}

Token Lexer::nextToken()
{
  this->skipWhiteSpace();

  size_t line = state_.line;
  size_t col = state_.col;

  if (state_.curr == '\0')
    return Token{TokenType::TOKEN_EOF, "", line, col};
  else if (state_.curr == '"' || state_.curr == '\'') 
    return parseQuotedString();
  else if (std::isalnum(state_.curr) || state_.curr == '.' || state_.curr == '_' || state_.curr == '$' || state_.curr == '-')
    return parseWord();
  else
    return parseOperator();
}

Token Lexer::parseWord()
{
  size_t start = state_.pos;
  size_t line = state_.line;
  size_t col = state_.col;

  while (state_.curr != '\0' && !std::isspace(state_.curr) &&
         state_.curr != '|' && state_.curr != '&' && state_.curr != ';' &&
         state_.curr != '<' && state_.curr != '>' && state_.curr != '(' &&
         state_.curr != ')')
  {
    advance();
  }
  return Token{TokenType::TOKEN_WORD,
               state_.input.substr(start, state_.pos - start), line, col};
}

Token Lexer::parseOperator()
{
  size_t line = state_.line;
  size_t col = state_.col;

  char c = state_.curr;
  this->advance();

  switch (c)
  {
  case '|':
    if (state_.curr == '|')
    {
      this->advance();
      return Token{TokenType::TOKEN_OR_IF, "||", line, col};
    }
    return Token{TokenType::TOKEN_PIPE, "|", line, col};
  case '&':
    if (state_.curr == '&')
    {
      this->advance();
      return Token{TokenType::TOKEN_AND_IF, "&&", line, col};
    }
    return Token{TokenType::TOKEN_BACKGROUND, "&", line, col};
  case '>':
    if (state_.curr == '>')
    {
      this->advance();
      return Token{TokenType::TOKEN_REDIRECT_OUT_APPEND, ">>", line, col};
    }
    return Token{TokenType::TOKEN_REDIRECT_OUT, ">", line, col};
  case '<':
    return Token{TokenType::TOKEN_REDIRECT_IN, "<", line, col};
  case ';':
    return Token{TokenType::TOKEN_SEMI, ";", line, col};
  case '(':
    return Token{TokenType::TOKEN_LPAREN, "(", line, col};
  case ')':
    return Token{TokenType::TOKEN_RPAREN, ")", line, col};
  default:
    return Token{TokenType::TOKEN_WORD, std::string(1, c), line, col};
  }
}

Token Lexer::parseQuotedString() {
    char quoteChar = state_.curr; // ' or "
    advance(); // skip opening quote

    size_t startLine = state_.line;
    size_t startCol = state_.col;
    std::string value;

    while (state_.curr != '\0' && state_.curr != quoteChar) {
        value += state_.curr;
        advance();
    }

    if (state_.curr == quoteChar) advance(); // skip closing quote

    return Token{TokenType::TOKEN_WORD, value, startLine, startCol};
}
