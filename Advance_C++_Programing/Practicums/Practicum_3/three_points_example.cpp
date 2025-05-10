#include <iostream>
// Хочется написать функцию, которая будет принимать неопределенное количество аргуметов разных типов и красиво их выводить

void printAll(){
    std::cout << std::endl;
}

template <typename T, typename... Types>
void printAll(T first, Types... args){
    std::cout << first << " ";
    printAll(args...);
}

int main(int argc, char** argv){
    printAll(2, 6.2, "Hello!");
}