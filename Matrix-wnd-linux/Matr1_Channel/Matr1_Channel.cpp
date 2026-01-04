#include "matrix_channel.h"
#include <iostream>
#include <chrono>

using namespace std;

int main() {
    int matrixSize = 5; 
    cout << "=== MULTIPLYING MATRICES WITH CHANNELS ===" << endl;
    cout << "Matrix size: " << matrixSize << "x" << matrixSize << endl << endl;

    
    for (int testSize = 5; testSize <= 8; testSize++) {
        cout << "\n=== Testing matrix " << testSize << "x" << testSize << " ===" << endl;

        MatrixCalculator calc(testSize);

        cout << "\nInitial matrices (first 10x10 shown):" << endl;
        calc.printAllMatrices();
        cout << endl;

        cout << "1. Simple multiplication:" << endl;
        auto start_simple = chrono::high_resolution_clock::now();
        calc.multiplySimple();
        auto end_simple = chrono::high_resolution_clock::now();
        auto duration_simple = chrono::duration_cast<chrono::microseconds>(end_simple - start_simple);

        
        cout << "Time: " << duration_simple.count() << " microseconds" << endl;
        if (testSize <= 8) {
            calc.printResult();
        }

        cout << endl << "2. Block multiplication with channels:" << endl;
        for (int blockSize = 1; blockSize <= min(testSize, 4); blockSize++) {
            MatrixCalculator blockCalc(testSize);
            cout << "Block size " << blockSize << ": ";

            auto start = chrono::high_resolution_clock::now();
            blockCalc.multiplyBlock(blockSize);
            auto end = chrono::high_resolution_clock::now();

            auto duration = chrono::duration_cast<chrono::microseconds>(end - start);
            cout << duration.count() << " microseconds" << endl;
        }
    }

    return 0;
}