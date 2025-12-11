#include <iostream>
#include <vector>
#include <chrono>
#include <pthread.h>
#include <iomanip>

struct BlockData {
    const std::vector<std::vector<int>>* A;
    const std::vector<std::vector<int>>* B;
    std::vector<std::vector<int>>* C;
    int start_row;
    int end_row;
    int start_col;
    int end_col;
    int N;
};

void sequentialMultiply(const std::vector<std::vector<int>>& A,
    const std::vector<std::vector<int>>& B,
    std::vector<std::vector<int>>& C, int N) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            C[i][j] = 0;
            for (int k = 0; k < N; k++) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

void* multiplyBlock(void* arg) {
    BlockData* data = static_cast<BlockData*>(arg);

    for (int i = data->start_row; i < data->end_row; i++) {
        for (int j = data->start_col; j < data->end_col; j++) {
            int sum = 0;
            for (int k = 0; k < data->N; k++) {
                sum += (*data->A)[i][k] * (*data->B)[k][j];
            }
            (*data->C)[i][j] = sum;
        }
    }

    return nullptr;
}

void parallelMultiply(const std::vector<std::vector<int>>& A,
    const std::vector<std::vector<int>>& B,
    std::vector<std::vector<int>>& C, int N, int k) {
    int num_blocks_row = (N + k - 1) / k;
    int num_blocks_col = (N + k - 1) / k;
    int total_blocks = num_blocks_row * num_blocks_col;

    std::vector<pthread_t> threads(total_blocks);
    std::vector<BlockData> block_data(total_blocks);

    int thread_idx = 0;
    for (int block_i = 0; block_i < num_blocks_row; block_i++) {
        for (int block_j = 0; block_j < num_blocks_col; block_j++) {
            int start_row = block_i * k;
            int end_row = std::min(start_row + k, N);
            int start_col = block_j * k;
            int end_col = std::min(start_col + k, N);

            block_data[thread_idx] = { &A, &B, &C, start_row, end_row, start_col, end_col, N };
            pthread_create(&threads[thread_idx], nullptr, multiplyBlock, &block_data[thread_idx]);
            thread_idx++;
        }
    }

    for (int i = 0; i < total_blocks; i++) {
        pthread_join(threads[i], nullptr);
    }
}

void printMatrix(const std::vector<std::vector<int>>& matrix, const std::string& name) {
    std::cout << name << ":" << std::endl;
    for (const auto& row : matrix) {
        for (int val : row) {
            std::cout << std::setw(3) << val << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

int main() {
    int N = 5;

    std::vector<std::vector<int>> A = { {1, 2, 3, 4, 5},
                                      {6, 7, 8, 9, 10},
                                      {11, 12, 13, 14, 15},
                                      {16, 17, 18, 19, 20},
                                      {21, 22, 23, 24, 25} };

    std::vector<std::vector<int>> B = { {25, 24, 23, 22, 21},
                                      {20, 19, 18, 17, 16},
                                      {15, 14, 13, 12, 11},
                                      {10, 9, 8, 7, 6},
                                      {5, 4, 3, 2, 1} };

    std::vector<std::vector<int>> C_seq(N, std::vector<int>(N, 0));
    std::vector<std::vector<int>> C_par(N, std::vector<int>(N, 0));

    std::cout << "Testing with N=5" << std::endl;
    std::cout << "=================" << std::endl;

    // Последовательное умножение
    auto start_seq = std::chrono::high_resolution_clock::now();
    sequentialMultiply(A, B, C_seq, N);
    auto end_seq = std::chrono::high_resolution_clock::now();
    auto duration_seq = std::chrono::duration_cast<std::chrono::microseconds>(end_seq - start_seq);

    std::cout << "Sequential time: " << duration_seq.count() << " microseconds" << std::endl;

    // Тестирование разных размеров блоков
    for (int k = 1; k <= N; k++) {
        std::fill(C_par.begin(), C_par.end(), std::vector<int>(N, 0));

        auto start_par = std::chrono::high_resolution_clock::now();
        parallelMultiply(A, B, C_par, N, k);
        auto end_par = std::chrono::high_resolution_clock::now();
        auto duration_par = std::chrono::duration_cast<std::chrono::microseconds>(end_par - start_par);

        int num_blocks_row = (N + k - 1) / k;
        int num_blocks_col = (N + k - 1) / k;
        int total_blocks = num_blocks_row * num_blocks_col;

        std::cout << "\nBlock size: " << k << ", Blocks: " << total_blocks
            << ", Threads: " << total_blocks << ", Time: " << duration_par.count() << " microseconds";

        // Проверка корректности
        bool correct = true;
        for (int i = 0; i < N && correct; i++) {
            for (int j = 0; j < N && correct; j++) {
                if (C_seq[i][j] != C_par[i][j]) {
                    correct = false;
                }
            }
        }

        if (correct) {
            std::cout << " ✓ CORRECT";
        }
        else {
            std::cout << " ✗ INCORRECT";
        }
    }

    std::cout << std::endl;

    // Вывод результатов для проверки
    printMatrix(A, "Matrix A");
    printMatrix(B, "Matrix B");
    printMatrix(C_seq, "Result (Sequential)");

    return 0;
}