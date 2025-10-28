#include "ShellContext.hpp"
#include<iostream>


int becho(const std::vector<std::string>& args)
{
    for (auto& arg:args) std::cout << arg << " ";
        std::cout << std::endl;
        return 0;
}
ShellContext::ShellContext()
{
    //register echo
   commandRegistry["echo"]  = becho;

}
