#include <iostream>
#include <functional>
#include<array>
#include<algorithm>
#include "Predicate.h"
int main() {
	std::array<int, 10> list{ 1,2,3,4,5,3,7,3,9,3 };
	int result= count_if(list.begin(), list.end(), Predicate(3));
	std::cout << result << std::endl;
	std::cout<<count_if(list.begin(), list.end(), Predicate(7))
	 << std::endl;

	int tmp = 7;
	result = count_if(list.begin(), list.end(), [tmp](int& item) {return item > tmp;});
		std::cout << result << std::endl;
		auto lambda = [tmp](int item) {return item > tmp;};
		result = count_if(list.begin(), list.end(), lambda);
 }