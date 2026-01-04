#include "matrix_thread.h"
#include <iostream>
#include <chrono>

using namespace std;

int main() {
    int matrixSize = 5;  
    cout << "=== MULTIPLYING MATRICES WITH THREADS ===" << endl;
    cout << "Matrix size: " << matrixSize << "x" << matrixSize << endl << endl;

    MatrixCalculator calc(matrixSize);

    cout << "Initial matrices:" << endl;
    calc.printAllMatrices();
    cout << endl;

    cout << "1. Simple multiplication:" << endl;
    calc.multiplySimple();
    calc.printResult();

    cout << endl << "2. Block multiplication with threads:" << endl;
    for (int blockSize = 1; blockSize <= matrixSize; blockSize++) {
        MatrixCalculator blockCalc(matrixSize);
        cout << "Block size " << blockSize << ": ";

        auto start = chrono::high_resolution_clock::now();
        blockCalc.multiplyBlock(blockSize);
        auto end = chrono::high_resolution_clock::now();

        auto duration = chrono::duration_cast<chrono::microseconds>(end - start);
        cout << duration.count() << " microseconds" << endl;
    }

    return 0;
}
