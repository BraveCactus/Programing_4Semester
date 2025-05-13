/*
Ленивый (lazy) и Энергичный (eager) C++ 
Опр: Eager Evaluation (Энергичные вычисления) - При таком подходе вычисления выполняются сразу, как только становится возможным.
Пример:
    int x = 10;  // вычисляется сразу
    class MyClass {

    public:
        MyClass() { std::cout << "Constructed!"; }
    };

    MyClass obj;  // объект создаётся сразу

    std::vector<int> vec = {1, 2, 3};  // выделяет память и заполняет элементы сразу

Опр: Lazy Evaluation (Ленивые вычисления) - При таком подходе вычисления откладываются до момента, когда результат действительно нужен.
Пример:
    auto lazyCompute = []() { return heavyCalculation(); };
    // Вычисление не происходит, пока не вызвана:
    int result = lazyCompute();

    или к примеру инстанцирование шаблонов


Опр: SFINAE (Substitution Failure Is Not An Error (провал подстановки не является ошибкой)) - 
    Если в результате подстановки в непосредственном контексте класса (функции, алиаса, переменной) возникает невалидная конструкция;
    То эта подстановка неуспешна, но не ошибочна

    template <typename T>
    T max(T x, T y){
        return (x > y)? x: y;
    }

    template <typename T, typename U>
    auto max(T x, U y){
        return (x > y)? x: y;
    }

    auto result = max(1.0, 3);

*/

#include <iostream>



template<typename T>
void foo(int T::*){
    std::cout << "Это класс" << std::endl;
}

template<typename... Types>
void foo(Types...){
    std::cout << "Это не класс" << std::endl;
}

struct A{};

// Теперь напишем класс, который будет проверять является ли тип классом

namespace implementation{
    template <typename T>
    static char test(int T::*);
    template <typename T>
    static int test(...);
}

template <class T>
struct is_class:
    std::integral_constant<bool, sizeof(implementation::test<T>(0)) == sizeof(char)>{};

struct A{};

//Продвинутое применение SFINAE

struct yes{char c;};
struct no {char c; yes cl;};

template <typename T> yes test_sort(decltype(&T::sort));
template <typename T> no test_sort(...);
template <typename T, size_t s> struct fast_sort_helper;

template <typename T>
struct fast_sort_helper<T, sizeof(yes)>{
    static void fast_sort(T& x){
        std::cout << "Сортируем T::sort" << std::endl;
        x.sort();
    }
};

template <typename T>

static void fast_sort(T& x){
    fast_sort_helper<T, sizeof(test_sort<T>(NULL))>::fast_sort(x);
}

class A{
public:
    void fast_sort(){}
};

class B{
public:
    void f(){}
};


int main(int argc, char** argv){
    // Классический пример применеия SFINAE - проверка является ли тип классом
    foo<double>(0);
    foo<A>(0);  
    
    // Пример со специальной структурой
    static_assert(is_class<int>::value, "int is a class?");
    static_assert(is_class<A>::value, "A not is a class?");
}