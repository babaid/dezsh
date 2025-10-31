#include "ShellContext.hpp"

class echoCmd: public ICommand
{
    int execute(ShellContext& context, std::vector<std::string>& args,
                    std::istream& in,
                    std::ostream& out) override;
    std::string name() const override {return "echo";};
};

class pwdCmd: public ICommand
{
    
    int execute(ShellContext& context, std::vector<std::string>& args,
                    std::istream& in,
                    std::ostream& out) override;
    std::string name() const override {return "pwd";};
};

class exitCmd: public ICommand
{
    int execute(ShellContext& context, std::vector<std::string>& args,
                    std::istream& in,
                    std::ostream& out) override;
    std::string name() const override {return "exit";};
};

class typeCmd: public ICommand
{
    int execute(ShellContext& context, std::vector<std::string>& args,
                    std::istream& in,
                    std::ostream& out) override;
    std::string name() const override {return "type";};
};


class cdCmd: public ICommand
{
    int execute(ShellContext& context, std::vector<std::string>& args,
                    std::istream& in,
                    std::ostream& out) override;
    std::string name() const override {return "cd";};
};

class exportCmd: public ICommand
{
     int execute(ShellContext& context, std::vector<std::string>& args,
                    std::istream& in,
                    std::ostream& out) override;
    std::string name() const override {return "export";};
};