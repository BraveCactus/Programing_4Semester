#include <iostream>
using namespace std;

// Функция, принимающая int и указатель на функцию (принимающую float и возвращающую int)
// Возвращает указатель на свой аргумент (опасно, т.к. arg - локальная переменная)
int* f(int arg, int (*)(float)) {
    return &arg;  // Опасное поведение: возврат указателя на локальную переменную
}

// Шаблонная функция, принимающая объект builder с методом create()
template <typename T>
auto makeAddProcess(const T& builder) -> decltype(builder.create()) {
    auto res = builder.create();
    return res;  // Возвращаем результат create(), а не новый вызов
}

// Шаблонная функция для вычисления
template <typename T>
float calc(const T arg) {
    return arg / 2.0f;
}

// Шаблонная функция с условием
template <typename T>
auto func(const T a) {
    if (a < 5) {
        return 0.0f;  // Все return должны возвращать один тип (float)
    } else {
        return calc(a);
    }
}

class Builder {
public:
    int create() const { return 42; }  // Реализация метода create()
};

void some_func(int a){
    cout << a << endl;
}

int main() {
    int a = 6;
    const int& ref_a = a;  // Константная ссылка на a

    // Указатель на функцию f
    int* (*ptr)(int, int (*)(float)) = &f;
    
    Builder builder;
    makeAddProcess(builder);  // Вызов шаблонной функции

    auto* s = &f;  // Автоматическое определение типа указателя на функцию
    auto& b = ref_a;  // Ссылка того же типа, что и ref_a

    cout << func(4) << endl;  // Выведет 0
    cout << func(6) << endl;  // Выведет 3

    auto d = &some_func;
    d(6);

    return 0;
}