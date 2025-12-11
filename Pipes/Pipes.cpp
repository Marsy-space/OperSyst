#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <unistd.h>
#include <sys/wait.h>
#include <cstring>

const int N = 28;

void process_M() {
    int x;
    while (std::cin >> x) {
        int result = x * 7;
        std::cerr << "[M] " << x << " * 7 = " << result << std::endl;
        std::cout << result << " ";
    }
    std::cerr << "M Завершил работу" << std::endl;
}

void process_A() {
    int x;
    while (std::cin >> x) {
        int result = x + N;
        std::cerr << "A " << x << " + " << N << " = " << result << std::endl;
        std::cout << result << " ";
    }
    std::cerr << "A Завершил работу" << std::endl;
}

void process_P() {
    int x;
    while (std::cin >> x) {
        int result = x * x * x;
        std::cerr << "[P] " << x << "^3 = " << result << std::endl;
        std::cout << result << " ";
    }
    std::cerr << "P Завершил работу" << std::endl;
}

void process_S() {
    int x;
    long long sum = 0;
    int count = 0;
    while (std::cin >> x) {
        count++;
        sum += x;
        std::cerr << "S Получено число " << count << ": " << x << ", текущая сумма: " << sum << std::endl;
    }
    std::cerr << "S ИТОГО: сумма " << count << " чисел = " << sum << std::endl;
    std::cout << sum << std::endl;
}

int main() {
    std::vector<int> numbers = {1, 2, 3};
    
    std::cerr << "Входные числа: ";
    for (int num : numbers) {
        std::cerr << num << " ";
    }
    std::cerr << std::endl;
    std::cerr << "Номер в списке N = " << N << std::endl;
    std::cerr << "Цепочка: Родитель -> M -> A -> P -> S" << std::endl << std::endl;
    
    int pipe_m_to_a[2];   
    int pipe_a_to_p[2];   
    int pipe_p_to_s[2];   
    int pipe_to_m[2];     
    
    if (pipe(pipe_to_m) == -1 || pipe(pipe_m_to_a) == -1 || 
        pipe(pipe_a_to_p) == -1 || pipe(pipe_p_to_s) == -1) {
        perror("pipe");
        return 1;
    }
    
    
    pid_t pid_m = fork();
    if (pid_m == 0) {
        std::cerr << "M Дочерний процесс M запущен: " << getpid() << ")" << std::endl;
        
        close(pipe_to_m[1]);    
        close(pipe_m_to_a[0]);  
        
        dup2(pipe_to_m[0], STDIN_FILENO);
        close(pipe_to_m[0]);
        
        dup2(pipe_m_to_a[1], STDOUT_FILENO);
        close(pipe_m_to_a[1]);
        
        process_M();
        std::cerr << "M Завершение" << std::endl;
        exit(0);
    }
    
    pid_t pid_a = fork();
    if (pid_a == 0) {
        std::cerr << "A Дочерний процесс A запущен: " << getpid() << ")" << std::endl;
        
        close(pipe_to_m[0]); close(pipe_to_m[1]);
        close(pipe_m_to_a[1]);  
        close(pipe_a_to_p[0]);  
        
        dup2(pipe_m_to_a[0], STDIN_FILENO);
        close(pipe_m_to_a[0]);
        
        dup2(pipe_a_to_p[1], STDOUT_FILENO);
        close(pipe_a_to_p[1]);
        
        process_A();
        std::cerr << "A Завершение" << std::endl;
        exit(0);
    }
    
    pid_t pid_p = fork();
    if (pid_p == 0) {
        std::cerr << "P Дочерний процесс P запущен: " << getpid() << ")" << std::endl;
        
        close(pipe_to_m[0]); close(pipe_to_m[1]);
        close(pipe_m_to_a[0]); close(pipe_m_to_a[1]);
        close(pipe_a_to_p[1]);  
        close(pipe_p_to_s[0]);  
        
        dup2(pipe_a_to_p[0], STDIN_FILENO);
        close(pipe_a_to_p[0]);
        
        dup2(pipe_p_to_s[1], STDOUT_FILENO);
        close(pipe_p_to_s[1]);
        
        process_P();
        std::cerr << "P Завершение" << std::endl;
        exit(0);
    }
    
    pid_t pid_s = fork();
    if (pid_s == 0) {
        std::cerr << "[S] Дочерний процесс S запущен: " << getpid() << ")" << std::endl;
        
        close(pipe_to_m[0]); close(pipe_to_m[1]);
        close(pipe_m_to_a[0]); close(pipe_m_to_a[1]);
        close(pipe_a_to_p[0]); close(pipe_a_to_p[1]);
        close(pipe_p_to_s[1]);  
        
        dup2(pipe_p_to_s[0], STDIN_FILENO);
        close(pipe_p_to_s[0]);
        
        process_S();
        std::cerr << "S Завершение" << std::endl;
        exit(0);
    }
    
    std::cerr << pid_m << ", A=" << pid_a 
              << ", P=" << pid_p << ", S=" << pid_s << std::endl;
    
    close(pipe_to_m[0]);  
    close(pipe_m_to_a[0]); close(pipe_m_to_a[1]);
    close(pipe_a_to_p[0]); close(pipe_a_to_p[1]);
    close(pipe_p_to_s[0]); close(pipe_p_to_s[1]);
    
    std::stringstream input;
    for (int num : numbers) {
        input << num << " ";
    }
    
    std::string input_str = input.str();
    std::cerr << "данные процессу M: \"" << input_str << "\"" << std::endl;
    
    write(pipe_to_m[1], input_str.c_str(), input_str.length());
    close(pipe_to_m[1]); 
    
    int status;
    waitpid(pid_m, &status, 0);
    std::cerr << "Процесс M завершился со статусом: " << WEXITSTATUS(status) << std::endl;
    
    waitpid(pid_a, &status, 0);
    std::cerr << "Процесс A завершился со статусом: " << WEXITSTATUS(status) << std::endl;
    
    waitpid(pid_p, &status, 0);
    std::cerr << "Процесс P завершился со статусом: " << WEXITSTATUS(status) << std::endl;
    
    waitpid(pid_s, &status, 0);
    std::cerr << "Процесс S завершился со статусом: " << WEXITSTATUS(status) << std::endl;
    
    std::cerr << "Все процессы завершены" << std::endl;
    
    std::cerr << "1. M: 1*7=7, 2*7=14, 3*7=21" << std::endl;
    std::cerr << "2. A: 7+28=35, 14+28=42, 21+28=49" << std::endl;
    std::cerr << "3. P: 35^3=42875, 42^3=74088, 49^3=117649" << std::endl;
    std::cerr << "4. S: 42875 + 74088 + 117649 = 234612" << std::endl;
    return 0;
}
