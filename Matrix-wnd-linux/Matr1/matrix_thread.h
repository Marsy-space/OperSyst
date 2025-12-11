#pragma once
#ifndef MATRIX_THREAD_H
#define MATRIX_THREAD_H

#include <vector>
#include <thread>

class MatrixCalculator {
public:
    std::vector<std::vector<int>> matrixA, matrixB, resultMatrix;
    int size;

    MatrixCalculator(int n);
    void fillMatrices();
    void multiplySimple();
    void multiplyBlock(int blockSize);
    void printResult();
    void printAllMatrices();

private:
    void multiplyBlockPart(int startRow, int endRow, int startCol, int endCol);
};

#endif