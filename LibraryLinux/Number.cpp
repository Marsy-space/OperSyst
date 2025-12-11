// Number.cpp : Определяет функции для статической библиотеки.
//

#include "Number.h"

Number::Number(double value) : value(value) {}
// Возвращает текущее значение числа
double Number::getValue() const {
    return value;
}

void Number::setValue(double value) {
    this->value = value;
}

Number Number::operator+(const Number& other) const {
    return Number(value + other.value);
}

Number Number::operator-(const Number& other) const {
    return Number(value - other.value);
}

Number Number::operator*(const Number& other) const {
    return Number(value * other.value);
}

Number Number::operator/(const Number& other) const {
    if (other.value == 0.0) {
        throw "Division by zero";// Генерируем исключение при делении на ноль
    }
    return Number(value / other.value);
}

const Number zero(0.0);// Глобальный ноль
const Number one(1.0);// Глобальная единица

Number createNumber(double value) {
    return Number(value);
}
