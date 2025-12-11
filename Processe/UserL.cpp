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
#include <unistd.h>

class ProcessUtils {
public:
    static bool isProcessRunning(const std::string& processName) {
        DIR* dir = opendir("/proc");

        if (!dir) {
            return false;
        }

        struct dirent* entry;
        while ((entry = readdir(dir)) != nullptr) {
            if (entry->d_type == DT_DIR && isNumber(entry->d_name)) {
                pid_t pid = std::stoi(entry->d_name);
                std::string procName = getProcessName(pid);

                if (!procName.empty() && procName == processName) {
                    closedir(dir);
                    return true;
                }
            }
        }

        closedir(dir);
        return false;
    }

    static bool isProcessRunning(pid_t processId) {
        return (kill(processId, 0) == 0);
    }

    static pid_t findProcessIdByName(const std::string& processName) {
        DIR* dir = opendir("/proc");

        if (!dir) {
            return 0;
        }

        struct dirent* entry;
        while ((entry = readdir(dir)) != nullptr) {
            if (entry->d_type == DT_DIR && isNumber(entry->d_name)) {
                pid_t pid = std::stoi(entry->d_name);
                std::string procName = getProcessName(pid);

                if (!procName.empty() && procName == processName) {
                    closedir(dir);
                    return pid;
                }
            }
        }

        closedir(dir);
        return 0;
    }

    static bool setEnvironmentVariable(const std::string& name, const std::string& value) {
        return setenv(name.c_str(), value.c_str(), 1) == 0;
    }

    static bool removeEnvironmentVariable(const std::string& name) {
        return unsetenv(name.c_str()) == 0;
    }

    static bool createProcess(const std::string& command) {
        int result = system(command.c_str());
        return (result == 0);
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

void demonstrateKiller() {
    std::cout << "=== Killer Application Demonstration ===" << std::endl;

    std::string procToKill = "gedit,gnome-calculator,kolourpaint";
    std::cout << "Setting PROC_TO_KILL = " << procToKill << std::endl;
    ProcessUtils::setEnvironmentVariable("PROC_TO_KILL", procToKill);

    std::cout << "\nStarting test processes..." << std::endl;
    system("gedit &");
    system("gnome-calculator &");
    system("kolourpaint &");

    sleep(2);

    std::cout << "\nChecking running processes:" << std::endl;
    std::vector<std::string> testProcesses = { "gedit", "gnome-calculator", "kolourpaint" };

    for (const auto& proc : testProcesses) {
        if (ProcessUtils::isProcessRunning(proc)) {
            std::cout << "  " << proc << " - RUNNING" << std::endl;
        }
        else {
            std::cout << "  " << proc << " - NOT RUNNING" << std::endl;
        }
    }

    std::cout << "\n--- Demonstration 1: Termination by name --" << std::endl;
    std::string targetProcess = "gedit";

    if (ProcessUtils::isProcessRunning(targetProcess)) {
        pid_t pid = ProcessUtils::findProcessIdByName(targetProcess);
        std::cout << "Before: " << targetProcess << " running (PID: " << pid << ")" << std::endl;

        std::string command = "./Killer --name " + targetProcess;
        ProcessUtils::createProcess(command);

        sleep(1);

        if (!ProcessUtils::isProcessRunning(targetProcess)) {
            std::cout << "After: " << targetProcess << " terminated - SUCCESS" << std::endl;
        }
        else {
            std::cout << "After: " << targetProcess << " still running - FAILED" << std::endl;
        }
    }

    std::cout << "\n--- Demonstration 2: Termination by ID --" << std::endl;
    targetProcess = "gnome-calculator";

    if (ProcessUtils::isProcessRunning(targetProcess)) {
        pid_t pid = ProcessUtils::findProcessIdByName(targetProcess);
        std::cout << "Before: " << targetProcess << " running (PID: " << pid << ")" << std::endl;

        std::string command = "./Killer --id " + std::to_string(pid);
        ProcessUtils::createProcess(command);

        sleep(1);

        if (!ProcessUtils::isProcessRunning(pid)) {
            std::cout << "After: process with PID " << pid << " terminated - SUCCESS" << std::endl;
        }
        else {
            std::cout << "After: process with PID " << pid << " still running - FAILED" << std::endl;
        }
    }

    std::cout << "\n--- Demonstration 3: Termination from PROC_TO_KILL --" << std::endl;
    targetProcess = "kolourpaint";

    if (ProcessUtils::isProcessRunning(targetProcess)) {
        pid_t pid = ProcessUtils::findProcessIdByName(targetProcess);
        std::cout << "Before: " << targetProcess << " running (PID: " << pid << ")" << std::endl;

        std::string command = "./Killer";
        ProcessUtils::createProcess(command);

        sleep(1);

        if (!ProcessUtils::isProcessRunning(targetProcess)) {
            std::cout << "After: " << targetProcess << " terminated - SUCCESS" << std::endl;
        }
        else {
            std::cout << "After: " << targetProcess << " still running - FAILED" << std::endl;
        }
    }

    std::cout << "\nRemoving environment variable PROC_TO_KILL..." << std::endl;
    ProcessUtils::removeEnvironmentVariable("PROC_TO_KILL");

    std::cout << "\n=== Demonstration completed ===" << std::endl;
}

int main() {
    std::cout << "User Application - Killer Process Demonstration" << std::endl;
    std::cout << "Make sure Killer is in the same directory!" << std::endl;

    char choice;
    std::cout << "Start demonstration? (y/n): ";
    std::cin >> choice;

    if (choice == 'y' || choice == 'Y') {
        demonstrateKiller();
    }
    else {
        std::cout << "Demonstration canceled" << std::endl;
    }

    std::cout << "Press Enter to exit...";
    std::cin.ignore();
    std::cin.get();

    return 0;
}
