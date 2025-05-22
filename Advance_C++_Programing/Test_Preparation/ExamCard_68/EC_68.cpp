// 68.	Как работает ключевое слово required? Когда проверятся ограничения?

/*
Опр: Ключевое слово requires в C++ используется для задания концептов (concepts) и проверки ограничений на шаблонные параметры. 
     Оно позволяет явно указывать, какие требования должны выполняться для типа или выражения.
*/

//Пример:

#include <iostream>
#include <concepts>

template <typename T>
concept HasPrint = requires(const T obj) {  // obj тоже const (для универсальности)
    obj.print();
};

template <HasPrint T>  // Используем концепт
void print(const T& x) {
    x.print();
}

class A {
private:
    int a;
public:
    void set(int x) {
        this->a = x;
    }

    void print() const { 
        std::cout << "Hello!" << std::endl;
    }
};

int main() {
    A l;
    l.print();  // Вызов метода класса A   
    return 0;
}