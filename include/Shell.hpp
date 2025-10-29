#ifndef SHELL
#define SHELL

#include "Lexer.hpp"
#include "AST.hpp"

class Shell
{
public:
    Shell();
    ~Shell();
    int run();

private:
    ShellContext context;
    static Shell* instance;

    static char **autocomplete(const char *input, int start, int end);
    static char *autocomplete_generate_wrapper(const char *input, int state);
    char *autocomplete_generate(const char *input, int state);
    static std::string buildPrompt(const ShellContext& context);
};

#endif