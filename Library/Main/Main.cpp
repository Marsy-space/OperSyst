#include <iostream>
#include "Number.h"
#include "Vector.h"

int main() {
    
    // Демонстрация работы с классом Number
    // Создаем два числа для тестирования 

    Number a = createNumber(28.0);
    Number b = createNumber(8.0);

    Number sum = a + b;
    Number diff = a - b;
    Number prod = a * b;
    Number quot = a / b;

    std::cout << "a = " << a.getValue() << ", b = " << b.getValue() << std::endl;
    std::cout << "a + b = " << sum.getValue() << std::endl;
    std::cout << "a - b = " << diff.getValue() << std::endl;
    std::cout << "a * b = " << prod.getValue() << std::endl;
    std::cout << "a / b = " << quot.getValue() << std::endl;


    Vector v1(createNumber(1.0), createNumber(0.0));
    Vector v2(createNumber(0.0), createNumber(1.0));
    Vector v3 = v1 + v2;

    std::cout << "v1: (" << v1.getX().getValue() << ", " << v1.getY().getValue() << ")" << std::endl;
    std::cout << "v2: (" << v2.getX().getValue() << ", " << v2.getY().getValue() << ")" << std::endl;
    std::cout << "v1 + v2: (" << v3.getX().getValue() << ", " << v3.getY().getValue() << ")" << std::endl;

    std::cout << "Polar coordinates of v3:" << std::endl;
    std::cout << "r = " << v3.getR().getValue() << ", p = " << v3.getPhi().getValue() << std::endl;

    std::cout << "Global zero vector: (" << zeroVector.getX().getValue() << ", " << zeroVector.getY().getValue() << ")" << std::endl;
    std::cout << "Global one vector: (" << oneVector.getX().getValue() << ", " << oneVector.getY().getValue() << ")" << std::endl;

    return 0;
}