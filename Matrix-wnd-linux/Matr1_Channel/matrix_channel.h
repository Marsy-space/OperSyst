#pragma once
#ifndef MATRIX_CHANNEL_H
#define MATRIX_CHANNEL_H

#include "buffered_channel.h"
#include <vector>
#include <thread>
#include <functional>
#include <tuple>

struct MatrixTask {
    int row_start;
    int row_end;
    int col_start;
    int col_end;
    int block_size;
};

struct MatrixResult {
    int row;
    int col;
    int value;
};

class MatrixCalculator {
private:
    int size;
    std::vector<std::vector<int>> matrixA;
    std::vector<std::vector<int>> matrixB;
    std::vector<std::vector<int>> resultMatrix;

    BufferedChannel<MatrixTask> task_channel;
    BufferedChannel<MatrixResult> result_channel;

    std::vector<std::thread> worker_threads;
    int num_workers;

public:
    MatrixCalculator(int n, int workers = 4, int channel_capacity = 100);
    ~MatrixCalculator();

    void fillMatrices();
    void multiplySimple();
    void multiplyBlock(int blockSize);
    void printResult();
    void printAllMatrices();

private:
    void multiplyBlockPart(int startRow, int endRow, int startCol, int endCol);
    void workerThread();
};
#endif
