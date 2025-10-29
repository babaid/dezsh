#include "ShellContext.hpp"

class echoCmd: public ICommand
{
    int execute(ShellContext& context, std::vector<std::string>& args) override;
    std::string name() const override {return "echo";};
};

class pwdCmd: public ICommand
{
    
    int execute(ShellContext& context, std::vector<std::string>& args) override;
    std::string name() const override {return "pwd";};
};

class exitCmd: public ICommand
{
    int execute(ShellContext& context, std::vector<std::string>& args) override;
    std::string name() const override {return "exit";};
};

class cdCmd: public ICommand
{
    int execute(ShellContext& context, std::vector<std::string>& args) override;
    std::string name() const override {return "cd";};
};
