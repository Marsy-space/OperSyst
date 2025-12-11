#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <tlhelp32.h>
#include <locale>

class ProcessUtils {
public:
    static bool isProcessRunning(const std::wstring& processName) {
        HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

        if (hSnapshot == INVALID_HANDLE_VALUE) {
            return false;
        }

        PROCESSENTRY32W pe;
        pe.dwSize = sizeof(PROCESSENTRY32W);

        if (Process32FirstW(hSnapshot, &pe)) {
            do {
                if (std::wstring(pe.szExeFile) == processName) {
                    CloseHandle(hSnapshot);
                    return true;
                }
            } while (Process32NextW(hSnapshot, &pe));
        }

        CloseHandle(hSnapshot);
        return false;
    }

    static bool isProcessRunning(DWORD processId) {
        HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, processId);
        if (hProcess == NULL) {
            return false;
        }

        DWORD exitCode;
        if (GetExitCodeProcess(hProcess, &exitCode)) {
            CloseHandle(hProcess);
            return (exitCode == STILL_ACTIVE);
        }

        CloseHandle(hProcess);
        return false;
    }

    static DWORD findProcessIdByName(const std::wstring& processName) {
        HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

        if (hSnapshot == INVALID_HANDLE_VALUE) {
            return 0;
        }

        PROCESSENTRY32W pe;
        pe.dwSize = sizeof(PROCESSENTRY32W);

        if (Process32FirstW(hSnapshot, &pe)) {
            do {
                if (std::wstring(pe.szExeFile) == processName) {
                    CloseHandle(hSnapshot);
                    return pe.th32ProcessID;
                }
            } while (Process32NextW(hSnapshot, &pe));
        }

        CloseHandle(hSnapshot);
        return 0;
    }

    static bool setEnvironmentVariable(const std::wstring& name, const std::wstring& value) {
        return SetEnvironmentVariableW(name.c_str(), value.c_str()) != 0;
    }

    static bool removeEnvironmentVariable(const std::wstring& name) {
        return SetEnvironmentVariableW(name.c_str(), NULL) != 0;
    }

    static bool createProcess(const std::wstring& command) {
        STARTUPINFOW si = { sizeof(si) };
        PROCESS_INFORMATION pi;

        std::wstring cmd = L"cmd.exe /c " + command;

        if (CreateProcessW(
            NULL,
            const_cast<wchar_t*>(cmd.c_str()),
            NULL,
            NULL,
            FALSE,
            0,
            NULL,
            NULL,
            &si,
            &pi)
            ) {
            WaitForSingleObject(pi.hProcess, INFINITE);

            DWORD exitCode;
            GetExitCodeProcess(pi.hProcess, &exitCode);

            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);

            return exitCode == 0;
        }

        return false;
    }
};

void demonstrateKiller() {
    std::wcout << L"=== Демонстрация работы Killer ===" << std::endl;

    std::wstring procToKill = L"notepad,calc,mspaint";
    std::wcout << L"Устанавливаем PROC_TO_KILL = " << procToKill << std::endl;
    ProcessUtils::setEnvironmentVariable(L"PROC_TO_KILL", procToKill);

    std::wcout << L"\nЗапускаем тестовые процессы..." << std::endl;
    system("start notepad.exe");
    system("start calc.exe");
    system("start mspaint.exe");

    Sleep(2000);

    std::wcout << L"\nПроверка запущенных процессов:" << std::endl;
    std::vector<std::wstring> testProcesses = { L"notepad.exe", L"calc.exe", L"mspaint.exe" };

    for (const auto& proc : testProcesses) {
        if (ProcessUtils::isProcessRunning(proc)) {
            std::wcout << L"  " << proc << L" - ЗАПУЩЕН" << std::endl;
        }
        else {
            std::wcout << L"  " << proc << L" - НЕ ЗАПУЩЕН" << std::endl;
        }
    }

    std::wcout << L"\n--- Демонстрация 1: Завершение по имени --" << std::endl;
    std::wstring targetProcess = L"notepad.exe";

    if (ProcessUtils::isProcessRunning(targetProcess)) {
        DWORD pid = ProcessUtils::findProcessIdByName(targetProcess);
        std::wcout << L"До: " << targetProcess << L" запущен (PID: " << pid << L")" << std::endl;

        std::wstring command = L"Killer.exe --name " + targetProcess;
        ProcessUtils::createProcess(command);

        Sleep(1000);

        if (!ProcessUtils::isProcessRunning(targetProcess)) {
            std::wcout << L"После: " << targetProcess << L" завершен - УСПЕХ" << std::endl;
        }
        else {
            std::wcout << L"После: " << targetProcess << L" все еще запущен - НЕУДАЧА" << std::endl;
        }
    }

    std::wcout << L"\n--- Демонстрация 2: Завершение по ID --" << std::endl;
    targetProcess = L"calc.exe";

    if (ProcessUtils::isProcessRunning(targetProcess)) {
        DWORD pid = ProcessUtils::findProcessIdByName(targetProcess);
        std::wcout << L"До: " << targetProcess << L" запущен (PID: " << pid << L")" << std::endl;

        std::wstring command = L"Killer.exe --id " + std::to_wstring(pid);
        ProcessUtils::createProcess(command);

        Sleep(1000);

        if (!ProcessUtils::isProcessRunning(pid)) {
            std::wcout << L"После: процесс с PID " << pid << L" завершен - УСПЕХ" << std::endl;
        }
        else {
            std::wcout << L"После: процесс с PID " << pid << L" все еще запущен - НЕУДАЧА" << std::endl;
        }
    }

    std::wcout << L"\n--- Демонстрация 3: Завершение из PROC_TO_KILL --" << std::endl;
    targetProcess = L"mspaint.exe";

    if (ProcessUtils::isProcessRunning(targetProcess)) {
        DWORD pid = ProcessUtils::findProcessIdByName(targetProcess);
        std::wcout << L"До: " << targetProcess << L" запущен (PID: " << pid << L")" << std::endl;

        std::wstring command = L"Killer.exe";
        ProcessUtils::createProcess(command);

        Sleep(1000);

        if (!ProcessUtils::isProcessRunning(targetProcess)) {
            std::wcout << L"После: " << targetProcess << L" завершен - УСПЕХ" << std::endl;
        }
        else {
            std::wcout << L"После: " << targetProcess << L" все еще запущен - НЕУДАЧА" << std::endl;
        }
    }

    std::wcout << L"\nУдаляем переменную окружения PROC_TO_KILL..." << std::endl;
    ProcessUtils::removeEnvironmentVariable(L"PROC_TO_KILL");

    std::wcout << L"\n=== Демонстрация завершена ===" << std::endl;
}

// Изменяем wmain на main
int main() {
    setlocale(LC_ALL, "Russian");

    std::wcout << L"Приложение User - Демонстрация работы процесса Killer" << std::endl;
    std::wcout << L"Убедитесь, что Killer.exe находится в той же директории!" << std::endl;

    char choice;
    std::cout << "Запустить демонстрацию? (y/n): ";
    std::cin >> choice;

    if (choice == 'y' || choice == 'Y') {
        demonstrateKiller();
    }
    else {
        std::wcout << L"Демонстрация отменена" << std::endl;
    }

    std::wcout << L"Нажмите Enter для выхода...";
    std::cin.ignore();
    std::cin.get();

    return 0;
}