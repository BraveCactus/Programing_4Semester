#include <iostream>
#include <memory>

template <typename T>
class MyUnique {
    T* ptr_ = nullptr;

    // Удаляем копирующий конструктор и оператор присваивания
    MyUnique(const MyUnique&) = delete;
    MyUnique& operator=(const MyUnique&) = delete;

public:
    // Конструктор
    MyUnique(T* new_ptr) : ptr_{new_ptr} {}

    // Деструктор
    ~MyUnique() {
        delete ptr_; // Освобождение памяти
    }

    // Оператор доступа к членам
    T* operator->() {
        return ptr_;
    }

    const T* operator->() const {
        return ptr_;
    }
};

class MyClass {
public:
    int a;
    MyClass(int v) : a{v} {} // Исправленный конструктор
};

template <typename T, typename... Args>
MyUnique<T> MakeUnique(Args... args) {
    return MyUnique<T>(new T(args...)); // Исправлено создание объекта
}

class OtherClass {
public:
    MyUnique<MyClass> value; // Объявление указателя

    OtherClass() : value(new MyClass(0)) {} // Инициализация в конструкторе
};

int main() {
    OtherClass smth;
    smth.value->a = 7; // Изменение значения a
    std::cout << smth.value->a << std::endl; // Вывод значения a

    auto made_ptr = MakeUnique<MyClass>(10); // Создание уникального указателя
    std::cout << made_ptr->a << std::endl; // Вывод значения a

    return 0; // Память будет автоматически освобождена при выходе из области видимости
}
