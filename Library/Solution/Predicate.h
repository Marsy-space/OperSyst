#pragma once
class Predicate
{
public:
	Predicate(const int& value);
	bool operator()(const int& v);
private:
	int criterion;
};

