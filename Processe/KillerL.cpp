#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <cstdlib>
#include <cstring>
#include <signal.h>
#include <sys/types.h>
#include <dirent.h>
#include <fstream>

class ProcessKiller {
public:
    static bool killProcessById(pid_t processId) {
        if (kill(processId, SIGTERM) == 0) {
            std::cout << "Process with ID " << processId << " successfully terminated" << std::endl;
            return true;
        }
        else {
            std::cout << "Failed to terminate process with ID: " << processId << std::endl;
            return false;
        }
    }

    static bool killProcessByName(const std::string& processName) {
        bool found = false;
        DIR* dir = opendir("/proc");

        if (!dir) {
            std::cout << "Failed to open /proc directory" << std::endl;
            return false;
        }

        struct dirent* entry;
        while ((entry = readdir(dir)) != nullptr) {
          
            if (entry->d_type == DT_DIR && isNumber(entry->d_name)) {
                pid_t pid = std::stoi(entry->d_name);
                std::string procName = getProcessName(pid);

                if (!procName.empty() && procName == processName) {
                    std::cout << "Found process: " << procName << " (ID: " << pid << ")" << std::endl;
                    if (killProcessById(pid)) {
                        found = true;
                    }
                }
            }
        }

        closedir(dir);

        if (!found) {
            std::cout << "Process with name '" << processName << "' not found" << std::endl;
        }

        return found;
    }

    static std::vector<std::string> splitString(const std::string& str, char delimiter) {
        std::vector<std::string> tokens;
        std::stringstream ss(str);
        std::string token;

        while (std::getline(ss, token, delimiter)) {
            if (!token.empty()) {
                tokens.push_back(token);
            }
        }

        return tokens;
    }

    static void killProcessesFromEnvironment() {
        char* envValue = std::getenv("PROC_TO_KILL");
        if (envValue == nullptr) {
            std::cout << "Environment variable PROC_TO_KILL not found" << std::endl;
            return;
        }

        std::cout << "PROC_TO_KILL: " << envValue << std::endl;

        std::vector<std::string> processNames = splitString(envValue, ',');

        for (const auto& processName : processNames) {
            std::cout << "Terminating processes with name: " << processName << std::endl;
            killProcessByName(processName);
        }
    }

private:
    static bool isNumber(const std::string& str) {
        for (char c : str) {
            if (!std::isdigit(c)) return false;
        }
        return !str.empty();
    }

    static std::string getProcessName(pid_t pid) {
        std::string path = "/proc/" + std::to_string(pid) + "/comm";
        std::ifstream file(path);
        std::string name;

        if (file.is_open()) {
            std::getline(file, name);
            return name;
        }
        return "";
    }
};

void printUsage() {
    std::cout << "Usage:" << std::endl;
    std::cout << "  ./Killer --id <process_id>    - terminate process by ID" << std::endl;
    std::cout << "  ./Killer --name <process_name> - terminate all processes by name" << std::endl;
    std::cout << "  ./Killer                      - terminate processes from PROC_TO_KILL" << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc == 1) {
        std::cout << "Terminating processes from environment variable PROC_TO_KILL..." << std::endl;
        ProcessKiller::killProcessesFromEnvironment();
    }
    else if (argc == 3) {
        std::string param = argv[1];

        if (param == "--id") {
            pid_t processId = std::stoi(argv[2]);
            std::cout << "Terminating process with ID: " << processId << std::endl;
            ProcessKiller::killProcessById(processId);
        }
        else if (param == "--name") {
            std::string processName = argv[2];
            std::cout << "Terminating processes with name: " << processName << std::endl;
            ProcessKiller::killProcessByName(processName);
        }
        else {
            std::cout << "Unknown parameter: " << param << std::endl;
            printUsage();
            return 1;
        }
    }
    else {
        std::cout << "Invalid number of parameters" << std::endl;
        printUsage();
        return 1;
    }

    return 0;
}
