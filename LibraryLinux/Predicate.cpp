#include "Predicate.h"

Predicate::Predicate(const int& value) :criterion(value) {}

bool Predicate::operator()(const int& v) {
	return v > criterion;
}