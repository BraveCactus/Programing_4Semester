/*
Опр: ODR (One Defenition Rule) - сколь угодно declaration, не более чем один definition
Разрешения перегрузки:
    Для обычных функций:
        1) Идеальное совпадение выигрывает
        2) Все стандартные преобразования равны
        3) Троеточие проигрывает всему
    Для шаблонных функций:
        1) Точно подходящая функция выигрывает у шаблона
        2) Более спец шаблон выиграывает у менее спец шаблона
        3) Меньшее кол-во аргументов выигрывает против большего
        4) Пользовательский тип проигрывает встроенному

*/

#include <iostream>
using namespace std;

template <typename T> T do_power(T x, T acc, unsigned n){
    while(n > 0){
        if ((n & 0x1) == 0x1) {acc *= x; n -= 1;}
        else {x *= x; n/=2;}
    }
    return acc;
}

unsigned power(unsigned x, unsigned n){
    
    if((x < 2u) || (n == 1u)) return x;    
        return do_power<unsigned>(x, 1u, n);
}

// Параметр по умолчанию шаблона
// double в данном шаблоне это важно! 
template<typename T = double> int foo(T x = 1.0){
    return 42;
}

int main(){

    int a = 2;
    int n = 10;

    int result = power(a, n);

    cout << result << endl;

    int x = foo(1);
    int y = foo();

}