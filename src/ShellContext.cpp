#include "BuiltinCommands.hpp"
#include "Helpers.hpp"

#include <print>
#include <stdexcept>
#include <unistd.h>

ShellContext::ShellContext()
{
        current_dir = std::filesystem::current_path();
        // register echo
        commandRegistry.emplace("echo", std::make_unique<echoCmd>());
        commandRegistry.emplace("pwd", std::make_unique<pwdCmd>());
        commandRegistry.emplace("exit", std::make_unique<exitCmd>());
        commandRegistry.emplace("type", std::make_unique<typeCmd>());
        commandRegistry.emplace("cd", std::make_unique<cdCmd>());
        commandRegistry.emplace("export", std::make_unique<exportCmd>());

        env.emplace("HOME", getenv("HOME"));
        env.emplace("PATH", getenv("PATH"));
        env.emplace("USER", getenv("USER"));

        char chhostname[256];
        gethostname(chhostname, sizeof(chhostname));
        hostname = chhostname;
}

std::filesystem::path ShellContext::find_executable(const std::string &cmd)
{
        std::filesystem::path exec_file{};
        // different for windows
        std::vector<std::string> PATH = Helpers::split(env["PATH"].c_str(), ':');
        for (const auto &dir : PATH)
        {
                for (const auto &entry : std::filesystem::recursive_directory_iterator(dir, std::filesystem::directory_options::skip_permission_denied))
                {
                        if (entry.path().filename() == cmd)
                        {
                                if (access(entry.path().c_str(), X_OK) == 0)
                                {
                                        exec_file = entry.path();
                                        return exec_file;
                                }
                        }
                }
        }
        return exec_file;
}

int ShellContext::executeExternalCommand(std::string &CmdName, std::vector<std::string> &args,
                    std::istream& in,
                    std::ostream& out)
{
        fs::path exe_path = find_executable(CmdName);
        std::string exec_cmd = exe_path.string();

        std::vector<char *> argv;

        argv.push_back(CmdName.data());

        for (auto &arg : args)
        {
                argv.push_back(arg.data());
        }
        argv.push_back(nullptr);

        if (!exe_path.empty())
        {
                size_t pid = fork();
                if (pid == 0)
                {
                        int err_flag = execv(exec_cmd.data(), argv.data());
                        if (err_flag)
                        {
                                perror("Could not execute.");
                                exit_code = err_flag;
                        }
                }
                else if (pid > 0)
                {
                        int status;
                        waitpid(pid, &status, 0);
                }
        }
        else
                std::println("{}: not found", CmdName);
        return 0;
};
