#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <windows.h>
#include <thread>

const int N = 28;


DWORD WINAPI Process_M(LPVOID lpParam) {
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);

    char buffer[256];
    DWORD bytesRead;

    while (ReadFile(hStdin, buffer, sizeof(buffer), &bytesRead, NULL) && bytesRead > 0) {
        std::string input(buffer, bytesRead);
        std::stringstream ss(input);
        std::stringstream output;

        int x;
        while (ss >> x) {
            output << x * 7 << " ";
        }

        std::string result = output.str();
        DWORD bytesWritten;
        WriteFile(hStdout, result.c_str(), result.length(), &bytesWritten, NULL);
    }

    return 0;
}

DWORD WINAPI Process_A(LPVOID lpParam) {
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);

    char buffer[256];
    DWORD bytesRead;

    while (ReadFile(hStdin, buffer, sizeof(buffer), &bytesRead, NULL) && bytesRead > 0) {
        std::string input(buffer, bytesRead);
        std::stringstream ss(input);
        std::stringstream output;

        int x;
        while (ss >> x) {
            output << x + N << " ";
        }

        std::string result = output.str();
        DWORD bytesWritten;
        WriteFile(hStdout, result.c_str(), result.length(), &bytesWritten, NULL);
    }

    return 0;
}

int main() {
    setlocale(LC_ALL, "Russian");
    
    std::vector<int> numbers = { 1, 2, 3 };

    std::stringstream m_out, a_out, p_out;

    for (int num : numbers) {
        m_out << num * 7 << " ";
    }
    std::cout << "После M: " << m_out.str() << std::endl;

    int x;
    while (m_out >> x) {
        a_out << x + N << " ";
    }
    std::cout << "После A (+" << N << "): " << a_out.str() << std::endl;

    while (a_out >> x) {
        p_out << x * x * x << " ";
    }
    std::cout << "После P (x^3): " << p_out.str() << std::endl;

    long long sum = 0;
    while (p_out >> x) {
        sum += x;
    }
    std::cout << "После S: " << sum << std::endl;

    std::cout << "\nИтоговый результат: " << sum << std::endl;

    std::stringstream input_stream;
    for (int num : numbers) {
        input_stream << num << " ";
    }

    std::stringstream pipe1, pipe2, pipe3;

    while (input_stream >> x) {
        pipe1 << x * 7 << " ";
    }

    while (pipe1 >> x) {
        pipe2 << x + N << " ";
    }

    while (pipe2 >> x) {
        pipe3 << x * x * x << " ";
    }

    sum = 0;
    while (pipe3 >> x) {
        sum += x;
    }

    std::cout << "Результат через строковые потоки: " << sum << std::endl;

    return 0;
}
    
    