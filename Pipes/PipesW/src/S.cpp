#include <iostream>
#include <sstream>
#include <string>
#include <windows.h>

int main() {
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);

    char buffer[1024];
    DWORD bytesRead;
    long long sum = 0;

    while (ReadFile(hStdin, buffer, sizeof(buffer) - 1, &bytesRead, NULL) && bytesRead > 0) {
        buffer[bytesRead] = '\0';
        std::string input(buffer);
        std::stringstream ss(input);

        int x;
        while (ss >> x) {
            sum += x;
        }

        if (bytesRead < sizeof(buffer) - 1) {
            break;
        }
    }

    
    std::string result = std::to_string(sum) + "\n";
    DWORD bytesWritten;
    WriteFile(hStdout, result.c_str(), result.length(), &bytesWritten, NULL);

    return 0;
}