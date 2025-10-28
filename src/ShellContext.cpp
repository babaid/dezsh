#include "ShellContext.hpp"
#include<iostream>
#include<stdexcept>

int becho(ShellContext& context, const std::vector<std::string>& args)
{
    for (auto& arg:args) std::cout << arg << " ";
        std::cout << std::endl;
        return 0;
}
int bpwd(ShellContext& context,const std::vector<std::string>& args)
{
    if (!args.empty()) throw std::runtime_error("pwd does not accpet any arguments.");
    std::cout << std::filesystem::current_path().string() << std::endl;
    return 0;
}

int bexit(ShellContext& context,const std::vector<std::string>& args)
{
    if (args.size()>=1) 
    {
        context.running = false;
        context.exit_code  = std::stoi(args[0]);
    }
    context.running = false;
    return 0;
}

ShellContext::ShellContext()
{
    current_dir = std::filesystem::current_path();
    //register echo
   commandRegistry["echo"]  = becho;
   commandRegistry["pwd"] = bpwd;
   commandRegistry["exit"] = bexit;

}
