#include <string>
#include <unordered_map>
#include <functional>
#include <filesystem>
#include <sys/wait.h>
#include <iostream>

namespace fs = std::filesystem;

class ShellContext;

class ICommand
{
public:
        virtual ~ICommand() = default;
        virtual int execute(ShellContext &context, std::vector<std::string> &args) = 0;
        virtual std::string name() const = 0;
        // virtual std::string help() const = 0;
};

struct ShellContext
{
        bool running = true;
        int exit_code;
        ShellContext();
        std::filesystem::path current_dir;
        std::unordered_map<std::string, std::unique_ptr<ICommand>>
            commandRegistry;
        std::unordered_map<std::string, std::string> env;
        std::string hostname;

        std::filesystem::path find_executable(const std::string &cmd);
        int executeExternalCommand(std::string &CmdName, std::vector<std::string> &args);
};
