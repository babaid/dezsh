#include <string>
#include <unordered_map>
#include<functional>


struct ShellContext {
  ShellContext();
  std::unordered_map<std::string, std::function<int(const std::vector<std::string>&)>>
      commandRegistry;
  std::unordered_map<std::string, std::string> env;
  int executeExternalCommand(std::string&, std::vector<std::string>&){return 0;};
};
