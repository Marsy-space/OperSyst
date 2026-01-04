#include "matrix_channel.h"
#include <iostream>
#include <chrono>
#include <algorithm>
#include <random>

using namespace std;

MatrixCalculator::MatrixCalculator(int n, int workers, int channel_capacity)
    : size(max(n, 5)),  
    num_workers(workers),
    task_channel(channel_capacity),
    result_channel(channel_capacity) {

    
    matrixA.resize(size, vector<int>(size));
    matrixB.resize(size, vector<int>(size));
    resultMatrix.resize(size, vector<int>(size, 0));
    fillMatrices();

    
    for (int i = 0; i < num_workers; ++i) {
        worker_threads.emplace_back(&MatrixCalculator::workerThread, this);
    }
}

MatrixCalculator::~MatrixCalculator() {
    
    task_channel.close();
    result_channel.close();

   
    for (auto& t : worker_threads) {
        if (t.joinable()) t.join();
    }
}

void MatrixCalculator::fillMatrices() {
    
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(1, 10);

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            matrixA[i][j] = dis(gen);
            matrixB[i][j] = dis(gen);
        }
    }
}

void MatrixCalculator::workerThread() {
    while (true) {
        auto task_pair = task_channel.recv();
        if (!task_pair.second) break; 

        const MatrixTask& task = task_pair.first;
        multiplyBlockPart(task.row_start, task.row_end, task.col_start, task.col_end);
    }
}

void MatrixCalculator::multiplySimple() {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            resultMatrix[i][j] = 0;
            for (int k = 0; k < size; k++) {
                resultMatrix[i][j] += matrixA[i][k] * matrixB[k][j];
            }
        }
    }
}

void MatrixCalculator::multiplyBlock(int blockSize) {
   
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            resultMatrix[i][j] = 0;
        }
    }

    
    int task_count = 0;
    for (int startRow = 0; startRow < size; startRow += blockSize) {
        for (int startCol = 0; startCol < size; startCol += blockSize) {
            int endRow = min(startRow + blockSize, size);
            int endCol = min(startCol + blockSize, size);

            MatrixTask task{ startRow, endRow, startCol, endCol, blockSize };
            task_channel.send(std::move(task));
            task_count++;
        }
    }

   
    int results_needed = size * size;
    int results_received = 0;

    while (results_received < results_needed) {
        auto result_pair = result_channel.recv();
        if (!result_pair.second) break;

        const MatrixResult& result = result_pair.first;
        resultMatrix[result.row][result.col] = result.value;
        results_received++;
    }
}

void MatrixCalculator::multiplyBlockPart(int startRow, int endRow, int startCol, int endCol) {
    for (int i = startRow; i < endRow; i++) {
        for (int j = startCol; j < endCol; j++) {
            int sum = 0;
            for (int k = 0; k < size; k++) {
                sum += matrixA[i][k] * matrixB[k][j];
            }
            
            MatrixResult result{ i, j, sum };
            result_channel.send(std::move(result));
        }
    }
}

void MatrixCalculator::printResult() {
    cout << "Result matrix:" << endl;
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            cout << resultMatrix[i][j] << "\t";
        }
        cout << endl;
    }
}

void MatrixCalculator::printAllMatrices() {
    cout << "Matrix A (" << size << "x" << size << "):" << endl;
    for (int i = 0; i < min(size, 10); i++) { 
        for (int j = 0; j < min(size, 10); j++) { 
            cout << matrixA[i][j] << "\t";
        }
        if (size > 10) cout << "...";
        cout << endl;
    }
    if (size > 10) cout << "...\t...\t..." << endl;

    cout << endl << "Matrix B (" << size << "x" << size << "):" << endl;
    for (int i = 0; i < min(size, 10); i++) {
        for (int j = 0; j < min(size, 10); j++) {
            cout << matrixB[i][j] << "\t";
        }
        if (size > 10) cout << "...";
        cout << endl;
    }
    if (size > 10) cout << "...\t...\t..." << endl;
}