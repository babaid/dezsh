#include "Shell.hpp"

#include <readline/history.h>
#include <readline/readline.h>
#include <print>

Shell* Shell::instance = nullptr;

Shell::Shell(){
    instance = this;
}
Shell::~Shell()
{
    free(instance);
}

int Shell::run()
{
    rl_attempted_completion_function = Shell::autocomplete;
    rl_variable_bind("show-all-if-ambiguous", "on");

    while (context.running)
    {
        char *line = readline(":>");
        std::string line_str(line);
        free(line);
        if (line_str.empty())
            continue;
        try
        {
            Lexer lexer(line_str);
            auto tokens = lexer.tokenize();

            AST ast(tokens);
            ast.execute(context);
        }
        catch (std::exception &exc)
        {
            std::println("{}", exc.what());
        }
    }
    return context.exit_code;
}

char** Shell::autocomplete(const char* input, int start, int end) {
        rl_attempted_completion_over = 1;
        return rl_completion_matches(input, Shell::autocomplete_generate_wrapper);
}

char* Shell::autocomplete_generate(const char* input, int state) {
        static std::unordered_map<std::string, std::unique_ptr<ICommand>>::iterator it;
        static std::unordered_map<std::string, std::unique_ptr<ICommand>>::iterator end;
        static std::string prefix;

        if (state == 0) {
                it = context.commandRegistry.begin();
                end = context.commandRegistry.end();
                prefix = input;
        }

        char* name;

        while (it != end) {
                const std::string& name = it->first;
                ++it;

                if (name.rfind(prefix, 0) == 0) {
                        return strdup(name.c_str());
                }
        }

        return nullptr;
}

char* Shell::autocomplete_generate_wrapper(const char* input, int state) {
        if (Shell::instance) {
                return Shell::instance->autocomplete_generate(input, state);
        }
        return nullptr;
}