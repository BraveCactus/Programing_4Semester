#include <iostream>
#include <vector>
using namespace std;

// Агрегатная инициализация - инициализация структуры/класса с публичными полями
struct Simple {
    int a;
    float b;
public:
    Simple(int val1, int val2): a{val1}, b{val2}{}
};

Simple s(4, 5.7); // Инициализация с указанием имен полей

// Абстрактный базовый класс с чисто виртуальной функцией
class MyClassInterface {
public:
    virtual void func() = 0; // Чисто виртуальная функция
    MyClassInterface(){}
    virtual ~MyClassInterface() { cout << "MyClassInterface destructor" << endl; }
};

// Производный класс, реализующий абстрактный метод
class Derived : public MyClassInterface {
public:
    void func() override {} // Реализация виртуальной функции
    
    Derived() {}
    ~Derived() { cout << "Derived destructor" << endl; }
};

// Шаблонный класс для паттерна CRTP (Curiously Recurring Template Pattern)
template <typename T>
class CRTPBase {
public:
    void f() {
        static_cast<T*>(this)->g(); // Преобразование this к производному типу
    }
};

// Производный класс от CRTPBase
class CRTPDerived : public CRTPBase<CRTPDerived> {
    int a;
public:
    void g() { a = 0; } // Реализация метода, ожидаемого базовым классом
};

// Другой производный класс (исправленная версия)
class CRTPDerivedOther : public CRTPBase<CRTPDerivedOther> { // Исправлен шаблонный параметр
    float a;
public:
    void g() { a = 5.0f; } // Реализация метода g
};

int main() {
    // Работа с полиморфизмом
    Derived* d = new Derived();
    delete d; // Вызовется деструктор Derived, затем MyClassInterface
    
    cout << "-----" << endl;
    
    MyClassInterface* ptr = new Derived;
    delete ptr; // Благодаря виртуальному деструктору вызовется правильный деструктор
    
    cout << "-----" << endl;
    
    // Работа с CRTP
    CRTPDerived crtp1;
    crtp1.f(); // Вызовет g() из CRTPDerived
    
    CRTPDerivedOther crtp2;
    crtp2.f(); // Вызовет g() из CRTPDerivedOther
    
    return 0;
}