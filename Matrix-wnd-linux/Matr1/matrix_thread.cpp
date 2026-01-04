#include "matrix_thread.h"
#include <iostream>
#include <chrono>
#include <algorithm>

using namespace std;

MatrixCalculator::MatrixCalculator(int n) : size(n) {
    matrixA.resize(size, vector<int>(size));
    matrixB.resize(size, vector<int>(size));
    resultMatrix.resize(size, vector<int>(size, 0));
    fillMatrices();
}

void MatrixCalculator::fillMatrices() {
    vector<vector<int>> fixedA = {
        {1, 2, 3, 4, 5},
        {6, 7, 8, 9, 10},
        {11, 12, 13, 14, 15},
        {16, 17, 18, 19, 20},
        {21, 22, 23, 24, 25}
    };

    vector<vector<int>> fixedB = {
        {25, 24, 23, 22, 21},
        {20, 19, 18, 17, 16},
        {15, 14, 13, 12, 11},
        {10, 9, 8, 7, 6},
        {5, 4, 3, 2, 1}
    };

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            matrixA[i][j] = fixedA[i][j];
            matrixB[i][j] = fixedB[i][j];
        }
    }
}

void MatrixCalculator::printAllMatrices() {
    cout << "Matrix A:" << endl;
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            cout << matrixA[i][j] << "\t";
        }
        cout << endl;
    }

    cout << endl << "Matrix B:" << endl;
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            cout << matrixB[i][j] << "\t";
        }
        cout << endl;
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

    vector<thread> threads;

    for (int startRow = 0; startRow < size; startRow += blockSize) {
        for (int startCol = 0; startCol < size; startCol += blockSize) {
            int endRow = min(startRow + blockSize, size);
            int endCol = min(startCol + blockSize, size);

            thread newThread([=]() {
                multiplyBlockPart(startRow, endRow, startCol, endCol);
                });
            threads.push_back(move(newThread));
        }
    }

    for (auto& t : threads) {
        t.join();
    }
}

void MatrixCalculator::multiplyBlockPart(int startRow, int endRow, int startCol, int endCol) {
    for (int i = startRow; i < endRow; i++) {
        for (int j = startCol; j < endCol; j++) {
            int sum = 0;
            for (int k = 0; k < size; k++) {
                sum += matrixA[i][k] * matrixB[k][j];
            }
            resultMatrix[i][j] = sum;
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
