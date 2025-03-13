#include <iostream>

template<typename T>
void func(T&& var){
    std::cout << "func: " << var << std::endl;
}