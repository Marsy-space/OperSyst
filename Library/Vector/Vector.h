#pragma once

#ifdef VECTOR_EXPORTS
#define VECTOR_API __declspec(dllexport)
#else
#define VECTOR_API __declspec(dllimport)
#endif


#include "Number.h"

class VECTOR_API Vector {
private:
    Number x; // Координата X в декартовой системе
    Number y; // Координата Y в декартовой системе
public:
    Vector(Number x, Number y);
    Number getX() const;
    Number getY() const;
    void setX(Number x);
    void setY(Number y);

    // Методы для работы с полярными координатами
    Number getR() const;   
    Number getPhi() const; 

    
    Vector operator+(const Vector& other) const;
};
extern VECTOR_API const Vector zeroVector; // Нулевой вектор (0, 0)
extern VECTOR_API const Vector oneVector; // Вектор (1, 1)