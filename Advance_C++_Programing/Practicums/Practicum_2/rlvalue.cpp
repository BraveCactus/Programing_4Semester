#include <iostream>
#include <utility>
using namespace std;

class MyClass {
public:
    int* data;

    MyClass(int size) {
        data = new int[size];
        // Инициализация массива значениями
        for (int i = 0; i < size; ++i) {
            data[i] = i; // Пример инициализации
        }
        std::cout << "Resource allocated\n";
    }

    // Конструктор перемещения
    MyClass(MyClass& other) noexcept : data(other.data) {
        other.data = nullptr; // Освобождаем ресурс у другого объекта
        std::cout << "Resource moved\n";
    }

    // Удаляем конструктор копирования
    MyClass(const MyClass&) = delete;

    ~MyClass() {
        delete[] data; // Освобождение ресурса
        std::cout << "Resource deallocated\n";
    }

    void Print() {
        if (data) { // Проверка на nullptr
            cout << "data: " << *data << endl; // Вывод первого элемента
        } else {
            cout << "data is nullptr" << endl;
        }
    }
};

int main() {

    MyClass m(10);
    m.Print();

    MyClass&& q = move(m);
    q.Print();

    int&& z = 30;
    cout << z << endl;
    
    return 0;
}
