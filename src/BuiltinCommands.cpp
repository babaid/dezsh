#include "BuiltinCommands.hpp"
#include "Helpers.hpp"

#include <print>
#include <ranges>

int echoCmd::execute(ShellContext &context, std::vector<std::string> &args,
                     std::istream &in,
                     std::ostream &out)
{
        bool first = true;
        for (auto &&arg : args | std::views::join_with(' '))
        {
                out << arg;
        }
        out << std::endl;
        return 0;
}

int pwdCmd::execute(ShellContext &context, std::vector<std::string> &args,
                    std::istream &in,
                    std::ostream &out)
{
        out << context.current_dir.string() << std::endl;
        return 0;
}

int exitCmd::execute(ShellContext &context, std::vector<std::string> &args,
                     std::istream &in,
                     std::ostream &out)
{
        context.running = false;
        if (!args.empty())
                context.exit_code = std::stoi(args[0]);
        else
        context.exit_code = 0;
        return 0;
}

int typeCmd::execute(ShellContext &context, std::vector<std::string> &args,
                     std::istream &in,
                     std::ostream &out)
{
       if (!args.empty()) {
                std::cout << args.at(0);
                auto it = context.commandRegistry.find(args.at(0));
                if (it != context.commandRegistry.end()) {
                        out << " is a shell builtin" << std::endl;
                } else {
                        std::filesystem::path loc = context.find_executable(args.at(0));
                        if (!loc.empty()) {
                                out << " is " << loc.string() << std::endl;
                        } else
                                out << ": not found" << std::endl;
                }
        }
        else return 1;
        return 0;
}


int cdCmd::execute(ShellContext &context, std::vector<std::string> &args,
                   std::istream &in,
                   std::ostream &out)
{
        if (args.empty())
                std::filesystem::current_path(context.env["HOME"]);
        else if (args.size() == 1)
        {
                if (std::filesystem::is_directory(args.at(0)))
                {
                        std::filesystem::current_path(args.at(0));
                }
                else if (args.at(0).front() == '~')
                {
                        auto it = context.env.find("HOME");
                        if (it != context.env.end())
                        {
                                std::string rest(args.at(0).begin() + 1, args.at(0).end());
                                std::filesystem::path fullpath = it->second + rest;
                                std::filesystem::current_path(fullpath);
                        }
                        else
                        {

                                out << "Environment variable HOME is not set." << std::endl;
                                return 1;
                        }
                }
                else
                {

                        out << "cd: " << args[0] << ": No such file or directory" << std::endl; 
                        return 1;
                }
        }
        else
        {
                out << "cd: too many arguments" << std::endl;
                return 1;
        }

        context.current_dir = std::filesystem::current_path();
        return 0;
}

int exportCmd::execute(ShellContext &context, std::vector<std::string> &args,
                       std::istream &in,
                       std::ostream &out)
{
        for (auto &arg : args)
        {
                auto pos = arg.find("=");
                if (pos != std::string::npos)
                {
                        auto kv = Helpers::split(arg.c_str(), '=');
                        if (kv.size() == 2)
                                context.env[kv[0]] = kv[1];
                }
                else
                {
                        auto it = context.env.find(arg);
                        if (it == context.env.end())
                                context.env[arg] = "";
                }
        }
        return 0;
}