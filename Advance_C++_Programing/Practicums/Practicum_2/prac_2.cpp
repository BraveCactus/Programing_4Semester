
/*
Приведенный ниже код плох тем, что условно елси в функции foo произойдет ошибка то выбросится исключение и программа прервется,
а delete не сработает

class S{};

void foo(S* p){
    // Код
}

int main(int argc, char ** argv){
    S* p = new S;
    foo(p);
    delete p;
}

Опр: RAII (Resource Acquisition Is Initialization) - это идиома C++, при которой захват ресурса (память, файлы, мьютексы и т. д.) 
     происходит в конструкторе объекта, а освобождение — в деструкторе

Опр: std::move превращает l_value в r_value
Опр: RVO (Return Value Optimization) — это оптимизация компилятора, которая позволяет избежать лишнего копирования или перемещения
     объекта при возврате его из функции.

Опр: Правило пяти - при реализациии конструктора необходимо реализовать 
     1) Деструктор
     2) Конструктор копирования
     3) Оператор присваивания копированием 
     4) Конструктор перемещения
     5) Оператор присваивания перемещением
*/

#include <iostream>
#include <vector>
using namespace std;

template <typename T>
class ScopedPointer{
    T* ptr;
public:
    ScopedPointer(T* ptr_ = nullptr): ptr{ptr_}{};
    ~ScopedPointer() {delete ptr;};
    T& operator*() {return *ptr;};
    const T& operator*() const {return *ptr;};
};


int main(){ 
    int x1 = 10;
    int && y1 = move(x1);
    
    cout << x1 << endl;
    cout << y1 << endl;

    x1 = 20;

    cout << x1 << endl;
    cout << y1 << endl;

    int x = 4;
    int &&y = x + 1;

    cout << &x << " " << &y << endl;

    int &&z = move(x);
    z = z + 1;
    y = y + 1;
    cout << x << " " << y << endl;

    
}