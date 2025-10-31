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

int ShellContext::executeExternalCommand(
    std::string &CmdName,
    std::vector<std::string> &args,
    std::istream &in,
    std::ostream &out)
{

        fs::path exe_path = find_executable(CmdName);
        if (exe_path.empty())
        {
                std::println("{}: not found", CmdName);
                return -1;
        }

        std::string exec_cmd = exe_path.string();

        std::vector<char *> argv;
        argv.push_back(CmdName.data());
        for (auto &arg : args)
                argv.push_back(arg.data());
        argv.push_back(nullptr);

        // Read input from upstream command

        pid_t pid;
        int status;
        if (&in != &std::cin && in.rdbuf()->in_avail() > 0)
        {

                std::string inputStr((std::istreambuf_iterator<char>(in)),
                                     std::istreambuf_iterator<char>());
                // --- External command receiving piped input ---

                int pipefd[2];
                if (pipe(pipefd) == -1)
                {
                        perror("pipe");
                        return -1;
                }

                pid = fork();
                if (pid < 0)
                {
                        perror("fork");
                        return -1;
                }

                if (pid == 0)
                {
                        // Child: redirect stdin
                        close(pipefd[1]);
                        if (dup2(pipefd[0], STDIN_FILENO) == -1)
                        {
                                perror("dup2 stdin");
                                exit(1);
                        }
                        close(pipefd[0]);

                        execv(exec_cmd.data(), argv.data());
                        perror("execv failed");
                        exit(1);
                }
                else
                {
                        // Parent: write input to pipe
                        close(pipefd[0]);
                        if (!inputStr.empty())
                        {
                                ssize_t written = write(pipefd[1], inputStr.c_str(), inputStr.size());
                                if (written == -1)
                                        perror("write to pipe");
                        }
                        close(pipefd[1]); // signal EOF
                        waitpid(pid, &status, 0);
                }
        }
        else
        {
                // --- Standalone external command ---
                pid = fork();
                if (pid < 0)
                {
                        perror("fork");
                        return -1;
                }

                if (pid == 0)
                {
                        execv(exec_cmd.data(), argv.data());
                        perror("execv failed");
                        exit(1);
                }
                else
                {
                        waitpid(pid, &status, 0);
                }
        }

        exit_code = WIFEXITED(status) ? WEXITSTATUS(status) : -1;
        return exit_code;
}
