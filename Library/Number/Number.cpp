
#include "framework.h"
#include "Number.h"

Number::Number(double value) : value(value) {}

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
        throw "Division by zero";
    }
    return Number(value / other.value);
}

const Number zero(0.0);
const Number one(1.0);

Number createNumber(double value) {
    return Number(value);
}