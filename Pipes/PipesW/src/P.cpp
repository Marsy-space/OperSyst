#include <iostream>
#include <sstream>
#include <string>
#include <windows.h>

int main() {
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);

    char buffer[1024];
    DWORD bytesRead;

    while (ReadFile(hStdin, buffer, sizeof(buffer) - 1, &bytesRead, NULL) && bytesRead > 0) {
        buffer[bytesRead] = '\0';
        std::string input(buffer);
        std::stringstream ss(input);
        std::stringstream output;

        int x;
        while (ss >> x) {
            output << (x * x * x) << " ";
        }

        std::string result = output.str();
        DWORD bytesWritten;
        WriteFile(hStdout, result.c_str(), result.length(), &bytesWritten, NULL);

        if (bytesRead < sizeof(buffer) - 1) {
            break;
        }
    }

    DWORD bytesWritten;
    WriteFile(hStdout, "\n", 1, &bytesWritten, NULL);

    return 0;
}