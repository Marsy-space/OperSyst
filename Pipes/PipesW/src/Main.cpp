#include <iostream>
#include <vector>
#include <string>
#include <windows.h>

const int N = 28;

int main() {
    setlocale(LC_ALL, "Russian");

    std::vector<int> numbers = { 1, 2, 3, 4, 5 };

    std::cout << "=== ЦЕПОЧКА ПРОЦЕССОВ ===" << std::endl;
    std::cout << "Запуск процессов: M -> A -> P -> S" << std::endl;
    std::cout << "==========================" << std::endl;

    std::cout << "\nВходные данные: ";
    for (int num : numbers) {
        std::cout << num << " ";
    }
    std::cout << std::endl;
    std::cout << "N = " << N << std::endl;

    
    HANDLE hPipeMtoA[2];    // Pipe M -> A
    HANDLE hPipeAtoP[2];    // Pipe A -> P
    HANDLE hPipePtoS[2];    // Pipe P -> S
    HANDLE hPipeStoMain[2]; // Pipe S -> Main

    SECURITY_ATTRIBUTES sa = {
        sizeof(SECURITY_ATTRIBUTES),
        NULL,
        TRUE  
    };

    
    CreatePipe(&hPipeMtoA[0], &hPipeMtoA[1], &sa, 0);
    CreatePipe(&hPipeAtoP[0], &hPipeAtoP[1], &sa, 0);
    CreatePipe(&hPipePtoS[0], &hPipePtoS[1], &sa, 0);
    CreatePipe(&hPipeStoMain[0], &hPipeStoMain[1], &sa, 0);

    
    STARTUPINFO siS = { sizeof(STARTUPINFO) };
    PROCESS_INFORMATION piS;
    ZeroMemory(&siS, sizeof(siS));
    ZeroMemory(&piS, sizeof(piS));

    siS.cb = sizeof(STARTUPINFO);
    siS.hStdInput = hPipePtoS[0];
    siS.hStdOutput = hPipeStoMain[1];
    siS.hStdError = GetStdHandle(STD_ERROR_HANDLE);
    siS.dwFlags |= STARTF_USESTDHANDLES;

    CreateProcess("S.exe", NULL, NULL, NULL, TRUE, 0, NULL, NULL, &siS, &piS);

   
    STARTUPINFO siP = { sizeof(STARTUPINFO) };
    PROCESS_INFORMATION piP;
    ZeroMemory(&siP, sizeof(siP));
    ZeroMemory(&piP, sizeof(piP));

    siP.cb = sizeof(STARTUPINFO);
    siP.hStdInput = hPipeAtoP[0];
    siP.hStdOutput = hPipePtoS[1];
    siP.dwFlags |= STARTF_USESTDHANDLES;

    CreateProcess("P.exe", NULL, NULL, NULL, TRUE, 0, NULL, NULL, &siP, &piP);

   
    STARTUPINFO siA = { sizeof(STARTUPINFO) };
    PROCESS_INFORMATION piA;
    ZeroMemory(&siA, sizeof(siA));
    ZeroMemory(&piA, sizeof(piA));

    siA.cb = sizeof(STARTUPINFO);
    siA.hStdInput = hPipeMtoA[0];
    siA.hStdOutput = hPipeAtoP[1];
    siA.dwFlags |= STARTF_USESTDHANDLES;

    CreateProcess("A.exe", NULL, NULL, NULL, TRUE, 0, NULL, NULL, &siA, &piA);

   
    STARTUPINFO siM = { sizeof(STARTUPINFO) };
    PROCESS_INFORMATION piM;
    ZeroMemory(&siM, sizeof(siM));
    ZeroMemory(&piM, sizeof(piM));

    siM.cb = sizeof(STARTUPINFO);
    siM.hStdOutput = hPipeMtoA[1];
    siM.dwFlags |= STARTF_USESTDHANDLES;

    CreateProcess("M.exe", NULL, NULL, NULL, TRUE, 0, NULL, NULL, &siM, &piM);

    
    CloseHandle(hPipeMtoA[0]);
    CloseHandle(hPipeMtoA[1]);
    CloseHandle(hPipeAtoP[0]);
    CloseHandle(hPipeAtoP[1]);
    CloseHandle(hPipePtoS[0]);
    CloseHandle(hPipePtoS[1]);
    CloseHandle(hPipeStoMain[1]);

   
    std::string input_data;
    for (int num : numbers) {
        input_data += std::to_string(num) + " ";
    }
    input_data += "\n";

   
    HANDLE hStdinM = piM.hThread;

   
    std::cout << "\nОжидание результата..." << std::endl;

    char result_buffer[256];
    DWORD bytesRead;

    if (ReadFile(hPipeStoMain[0], result_buffer, sizeof(result_buffer) - 1, &bytesRead, NULL)) {
        result_buffer[bytesRead] = '\0';
        std::string result(result_buffer);

        
        while (!result.empty() && (result.back() == '\n' || result.back() == '\r' || result.back() == ' ')) {
            result.pop_back();
        }

        std::cout << "\n=== РЕЗУЛЬТАТ РАБОТЫ ЦЕПОЧКИ ===" << std::endl;
        std::cout << "Итоговая сумма: " << result << std::endl;

       
        std::cout << "\n=== ПРОВЕРОЧНЫЕ ВЫЧИСЛЕНИЯ ===" << std::endl;
        long long manual_sum = 0;
        for (int num : numbers) {
            int m = num * 7;
            int a = m + N;
            int p = a * a * a;
            manual_sum += p;

           
            std::cout << num << " -> M(" << num << "*7=" << m << ")"
                << " -> A(" << m << "+" << N << "=" << a << ")"
                << " -> P(" << a << "^3=" << p << ")" << std::endl;
        }
        std::cout << "Сумма вручную: " << manual_sum << std::endl;

        
        if (std::to_string(manual_sum) == result) {
            std::cout << "V Результаты совпадают!" << std::endl;
        }
        else {
            std::cout << "X Результаты не совпадают!" << std::endl;
        }
    }

   
    std::cout << "\nОжидание завершения процессов..." << std::endl;

    WaitForSingleObject(piM.hProcess, INFINITE);
    WaitForSingleObject(piA.hProcess, INFINITE);
    WaitForSingleObject(piP.hProcess, INFINITE);
    WaitForSingleObject(piS.hProcess, INFINITE);

   
    CloseHandle(piM.hProcess);
    CloseHandle(piM.hThread);
    CloseHandle(piA.hProcess);
    CloseHandle(piA.hThread);
    CloseHandle(piP.hProcess);
    CloseHandle(piP.hThread);
    CloseHandle(piS.hProcess);
    CloseHandle(piS.hThread);
    CloseHandle(hPipeStoMain[0]);

    std::cout << "\nВсе процессы завершены успешно." << std::endl;

    return 0;
}