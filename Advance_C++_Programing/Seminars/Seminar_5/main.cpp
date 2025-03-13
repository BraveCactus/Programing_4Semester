#include <iostream>
using namespace std;

int* f(int arg ,int (*)(float)){
    // cout << "Hi!" << endl;
    return &arg;
}

template <typename T>
auto makeAddProcess(const T& builder) -> decltype(builder.create()){
    auto res = builder.create();
    return decltype(builder.create())();
}

template <typename T>
float calc(const T arg){
    return arg / 2.0;
}

template <typename T>
auto func(const T a){
    //Важно чтобы return возвращал одинаковые типы данных, исправляем с помощью 0 -> 0.0f (чтобы тоже был float как и calc)
    if (a < 5){
        return 0.0f;
    } else {
        return calc(a);
    }

    if constexpr 
//Будет ошибка компиляции
}


class Builder{
public:
    int create() const; // Важно, чтобы был const
};

int main(){

    int a = 6;

    const int & ref_a = a;

    int* (*ptr)(int, int (*)(float)) = &f; //Указатель на функцию
    
    Builder builder;

    makeAddProcess(builder);

    auto*s = &f;

    auto& b = ref_a;

    cout << func(4) << endl;
}