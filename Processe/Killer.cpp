#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <tlhelp32.h>
#include <locale>

class ProcessKiller {
public:
    static bool killProcessById(DWORD processId) {
        HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, processId);
        if (hProcess == NULL) {
            std::wcout << L"Не удалось открыть процесс с ID: " << processId << std::endl;
            return false;
        }

        if (TerminateProcess(hProcess, 0)) {
            std::wcout << L"Процесс с ID " << processId << L" успешно завершен" << std::endl;
            CloseHandle(hProcess);
            return true;
        }
        else {
            std::wcout << L"Не удалось завершить процесс с ID: " << processId << std::endl;
            CloseHandle(hProcess);
            return false;
        }
    }

    static bool killProcessByName(const std::wstring& processName) {
        bool found = false;
        HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

        if (hSnapshot == INVALID_HANDLE_VALUE) {
            std::wcout << L"Не удалось создать снимок процессов" << std::endl;
            return false;
        }

        PROCESSENTRY32W pe;
        pe.dwSize = sizeof(PROCESSENTRY32W);

        if (Process32FirstW(hSnapshot, &pe)) {
            do {
                if (std::wstring(pe.szExeFile) == processName) {
                    std::wcout << L"Найден процесс: " << pe.szExeFile << L" (ID: " << pe.th32ProcessID << L")" << std::endl;
                    if (killProcessById(pe.th32ProcessID)) {
                        found = true;
                    }
                }
            } while (Process32NextW(hSnapshot, &pe));
        }

        CloseHandle(hSnapshot);

        if (!found) {
            std::wcout << L"Процесс с именем '" << processName << L"' не найден" << std::endl;
        }

        return found;
    }

    static std::vector<std::wstring> splitString(const std::wstring& str, wchar_t delimiter) {
        std::vector<std::wstring> tokens;
        std::wstringstream ss(str);
        std::wstring token;

        while (std::getline(ss, token, delimiter)) {
            if (!token.empty()) {
                tokens.push_back(token);
            }
        }

        return tokens;
    }

    static void killProcessesFromEnvironment() {
        wchar_t* envValue = _wgetenv(L"PROC_TO_KILL");
        if (envValue == nullptr) {
            std::wcout << L"Переменная окружения PROC_TO_KILL не найдена" << std::endl;
            return;
        }

        std::wcout << L"PROC_TO_KILL: " << envValue << std::endl;

        std::vector<std::wstring> processNames = splitString(envValue, L',');

        for (const auto& processName : processNames) {
            std::wcout << L"Завершение процессов с именем: " << processName << std::endl;
            killProcessByName(processName);
        }
    }
};

void printUsage() {
    std::wcout << L"Использование:" << std::endl;
    std::wcout << L"  Killer.exe --id <process_id>    - завершить процесс по ID" << std::endl;
    std::wcout << L"  Killer.exe --name <process_name> - завершить все процессы по имени" << std::endl;
    std::wcout << L"  Killer.exe                      - завершить процессы из PROC_TO_KILL" << std::endl;
}

int wmain(int argc, wchar_t* argv[]) {
    setlocale(LC_ALL, "Russian");

    if (argc == 1) {
        std::wcout << L"Завершение процессов из переменной окружения PROC_TO_KILL..." << std::endl;
        ProcessKiller::killProcessesFromEnvironment();
    }
    else if (argc == 3) {
        std::wstring param = argv[1];

        if (param == L"--id") {
            DWORD processId = std::stoul(argv[2]);
            std::wcout << L"Завершение процесса с ID: " << processId << std::endl;
            ProcessKiller::killProcessById(processId);
        }
        else if (param == L"--name") {
            std::wstring processName = argv[2];
            std::wcout << L"Завершение процессов с именем: " << processName << std::endl;
            ProcessKiller::killProcessByName(processName);
        }
        else {
            std::wcout << L"Неизвестный параметр: " << param << std::endl;
            printUsage();
            return 1;
        }
    }
    else {
        std::wcout << L"Неверное количество параметров" << std::endl;
        printUsage();
        return 1;
    }

    return 0;
}