#include "Shell.hpp"

#include <readline/history.h>
#include <readline/readline.h>
#include <print>

#include <unistd.h>    // gethostname
#include <pwd.h>       // getpwuid
#include <sys/types.h> // getpwuid
#include <string>
#include <filesystem>
#include <iostream>

Shell *Shell::instance = nullptr;

Shell::Shell()
{
    instance = this;
}
Shell::~Shell()
{
    instance = nullptr;
}

int Shell::run()
{
    rl_attempted_completion_function = Shell::autocomplete;
    rl_variable_bind("show-all-if-ambiguous", "on");
    while (context.running)
    {
        std::string prompt = Shell::buildPrompt(context);
        char *line = readline(prompt.c_str());
        if (!line) break;

        std::string line_str(line);
        free(line);

        if (line_str.empty() || line_str.find_first_not_of(" \t") == std::string::npos)
            continue;
        try
        {
            Lexer lexer(line_str);
            auto tokens = lexer.tokenize();

            AST ast(tokens);
            context.exit_code = ast.execute(context, std::cin, std::cout);
        }
        catch (std::exception &exc)
        {
            std::println("{}", exc.what());
        }
    }
    return context.exit_code;
}

char **Shell::autocomplete(const char *input, int start, int end)
{
    rl_attempted_completion_over = 1;
    return rl_completion_matches(input, Shell::autocomplete_generate_wrapper);
}

char *Shell::autocomplete_generate(const char *input, int state)
{
    static std::unordered_map<std::string, std::unique_ptr<ICommand>>::iterator it;
    static std::unordered_map<std::string, std::unique_ptr<ICommand>>::iterator end;
    static std::string prefix;

    if (state == 0)
    {
        it = context.commandRegistry.begin();
        end = context.commandRegistry.end();
        prefix = input;
    }

    char *name;

    while (it != end)
    {
        const std::string &name = it->first;
        ++it;

        if (name.rfind(prefix, 0) == 0)
        {
            return strdup(name.c_str());
        }
    }

    return nullptr;
}

char *Shell::autocomplete_generate_wrapper(const char *input, int state)
{
    if (Shell::instance)
    {
        return Shell::instance->autocomplete_generate(input, state);
    }
    return nullptr;
}

std::string color(const std::string &code) { return "\033[" + code + "m"; }
const std::string RESET = "\033[0m";

std::string Shell::buildPrompt(const ShellContext &context)
{
    std::string username;
    auto it = context.env.find("USER");
    if (it == context.env.end())
        username = "unknown";
    else
        username = it->second;

    // Directory, replace HOME with ~
    std::string dir = context.current_dir.string();
    std::string home = context.env.at("HOME");
    if (dir.rfind(home, 0) == 0)
    {
        dir = "~" + dir.substr(home.size());
    }

    std::string status_symbol = (context.exit_code == 0) ? "✔" : "✘";

    std::string prompt;
    prompt += color("34") + username + "@" + color("36") + context.hostname + color("0") + ":";
    prompt += color("32") + dir + color("0");
    prompt += " " + color("31") + status_symbol + color("0");
    prompt += " |~> ";

    return prompt;
}