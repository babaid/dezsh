#include "BuiltinCommands.hpp"
#include<iostream>
#include<print>
#include<ranges>

int echoCmd::execute(ShellContext& context, std::vector<std::string>& args)
{
     for (auto&& arg : args | std::views::join_with(' '))
       { std::print("{}", arg);}
       std::println();
return 0;
}

int pwdCmd::execute(ShellContext& context, std::vector<std::string>& args)
{
        std::println("{}", context.current_dir.string());
        return 0;
}

int exitCmd::execute(ShellContext& context, std::vector<std::string>& args)
{
     context.running = false;
     if (!args.empty()) context.exit_code = std::stoi(args[0]);
     else context.exit_code = 0;
     return 0;
}


int cdCmd::execute(ShellContext& context, std::vector<std::string>& args)
{
        if (args.empty())
                std::filesystem::current_path(context.env["HOME"]);
        else if (args.size() == 1) {
                if (std::filesystem::is_directory(args.at(0))) {
                        std::filesystem::current_path(args.at(0));
                } else
                        if(args.at(0).front() == '~')
                        {
                                auto it = context.env.find("HOME");
                                if (it != context.env.end())
                                {
                                        std::string rest(args.at(0).begin()+1, args.at(0).end());
                                        std::filesystem::path fullpath =  it->second + rest;
                                        std::filesystem::current_path(fullpath);
                                }
                                else{
                                       
                                        std::println("Environment variable HOME is not set.");
                                        return 1;
                                }
                        }
                        else{

                                std::println("cd: {}: No such file or directory", args[0]);
                                return 1;
                        }
        } else {
                std::println("cd: too many arguments");
                return 1;
        }

        context.current_dir =  std::filesystem::current_path();
        return 0;
}