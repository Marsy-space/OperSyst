#include <iostream>
#include <vector>
#include <thread>
#include <windows.h>
#include <chrono>

using namespace std;
using namespace std::chrono;

class Matrix {
public:
    vector<vector<int>> data;
    int size;

    Matrix(int n) : size(n), data(n, vector<int>(n, 0)) {}

    void fillRandom() {
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                data[i][j] = rand() % 10;
            }
        }
    }
};

struct ThreadData {
    Matrix* A;
    Matrix* B;
    Matrix* C;
    int blockSize;
    int rowBlock;
    int colBlock;
};

void multiplyBlock(ThreadData* data) {
    Matrix& A = *data->A;
    Matrix& B = *data->B;
    Matrix& C = *data->C;
    int blockSize = data->blockSize;
    int rowStart = data->rowBlock * blockSize;
    int colStart = data->colBlock * blockSize;

    int n = A.size;

    for (int i = rowStart; i < min(rowStart + blockSize, n); i++) {
        for (int j = colStart; j < min(colStart + blockSize, n); j++) {
            int sum = 0;
            for (int k = 0; k < n; k++) {
                sum += A.data[i][k] * B.data[k][j];
            }
            C.data[i][j] = sum;  
        }
    }
}

DWORD WINAPI windowsThread(LPVOID param) {
    ThreadData* data = (ThreadData*)param;
    multiplyBlock(data);
    return 0;
}

Matrix multiplyWindows(Matrix& A, Matrix& B, int blockSize) {
    int n = A.size;
    Matrix C(n);

    int blocks = (n + blockSize - 1) / blockSize;
    vector<HANDLE> threads;

    const int MAX_THREADS = 16;
    int activeThreads = 0;

    for (int i = 0; i < blocks; i++) {
        for (int j = 0; j < blocks; j++) {
            while (activeThreads >= MAX_THREADS) {
                Sleep(10);
            }

            ThreadData* data = new ThreadData{ &A, &B, &C, blockSize, i, j };
            HANDLE thread = CreateThread(NULL, 0, windowsThread, data, 0, NULL);
            if (thread) {
                threads.push_back(thread);
                activeThreads++;

                DWORD waitResult = WaitForSingleObject(thread, 0);
                if (waitResult == WAIT_OBJECT_0) {
                    CloseHandle(thread);
                    threads.pop_back();
                    activeThreads--;
                    delete data;
                }
            }
        }
    }

    WaitForMultipleObjects(threads.size(), threads.data(), TRUE, INFINITE);

    for (HANDLE thread : threads) {
        CloseHandle(thread);
    }

    return C;
}

Matrix multiplyStdThread(Matrix& A, Matrix& B, int blockSize) {
    int n = A.size;
    Matrix C(n);

    int blocks = (n + blockSize - 1) / blockSize;
    vector<thread> threads;    
    const int MAX_THREADS = 16;

    for (int i = 0; i < blocks; i++) {
        for (int j = 0; j < blocks; j++) {
            if (threads.size() >= MAX_THREADS) {
                threads[0].join();
                threads.erase(threads.begin());
            }

            threads.emplace_back([&A, &B, &C, blockSize, i, j]() {
                int rowStart = i * blockSize;
                int colStart = j * blockSize;
                int n = A.size;

                for (int i2 = rowStart; i2 < min(rowStart + blockSize, n); i2++) {
                    for (int j2 = colStart; j2 < min(colStart + blockSize, n); j2++) {
                        int sum = 0;
                        for (int k = 0; k < n; k++) {
                            sum += A.data[i2][k] * B.data[k][j2];
                        }
                        C.data[i2][j2] = sum;
                    }
                }
                });
        }
    }

    for (auto& t : threads) {
        t.join();
    }

    return C;
}

Matrix multiplySequential(Matrix& A, Matrix& B) {
    int n = A.size;
    Matrix C(n);

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            int sum = 0;
            for (int k = 0; k < n; k++) {
                sum += A.data[i][k] * B.data[k][j];
            }
            C.data[i][j] = sum;
        }
    }

    return C;
}

int main() {
    srand(time(0));
    const int N = 5;  

    cout << "Windows Version - Matrix Multiplication Benchmark" << endl;
    cout << "Matrix size: " << N << "x" << N << endl << endl;

    Matrix A(N), B(N);
    A.fillRandom();
    B.fillRandom();

    cout << "Sequential multiplication..." << endl;
    auto start = high_resolution_clock::now();
    Matrix C_seq = multiplySequential(A, B);
    auto end = high_resolution_clock::now();
    auto seq_time = duration_cast<milliseconds>(end - start).count();

    cout << "Sequential: " << seq_time << " ms" << endl << endl;

    cout << "Testing different block sizes..." << endl;
    cout << "Block Size | Blocks | Threads | std::thread (ms) | Windows (ms)" << endl;
    cout << "-----------|--------|---------|------------------|-------------" << endl;

   
    for (int blockSize = 2; blockSize <= N; blockSize += 2) {
        int blocks = (N + blockSize - 1) / blockSize;
        int totalThreads = blocks * blocks;

        cout << blockSize << "          | " << blocks << "      | " << totalThreads << "       | ";

       
        start = high_resolution_clock::now();
        Matrix C_std = multiplyStdThread(A, B, blockSize);
        end = high_resolution_clock::now();
        auto std_time = duration_cast<milliseconds>(end - start).count();
        cout << std_time << "                | ";

        
        start = high_resolution_clock::now();
        Matrix C_win = multiplyWindows(A, B, blockSize);
        end = high_resolution_clock::now();
        auto win_time = duration_cast<milliseconds>(end - start).count();
        cout << win_time << endl;
    }

    cout << "Benchmark completed successfully!" << endl;
    return 0;
}