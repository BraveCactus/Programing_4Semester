#include <iostream>
#include <vector>
using namespace std;

//Агрегатная инициализация

struct Simple{
    int a;
    float b;
};

Simple s = {.a = 4, .b = 5.7};

class MyClassInterface{
public:

    virtual void func() = 0;
    MyClassInterface() {}

    virtual ~MyClassInterface() { cout << "MyClassInterface" << endl;}
 
};

class Derived : public MyClassInterface {
public:
    void func () override {}

    Derived() {}
    ~Derived() {cout << "Derived" << endl;}

};

template <typename T>
class CRTPBase{
    
    void f () {
        static_cast<T>(this)->g(); 
    }

};

class CRTPDerived : public CRTPBase <CRTPDerived>{
    int a;
public:
    void g() {a = 0;}
};

class CRTPDerivedOther : public CRTPBase <CRTPDerived>{
    float a;
public:
    //void g() { a = 5.0;}
};

int main(){

    Derived *d = new Derived();

    delete d;

    MyClassInterface* ptr = new Derived;

    CRTPDerivedOther a;

    //a.f(); !!!Не скомпилируется

    delete ptr;

}