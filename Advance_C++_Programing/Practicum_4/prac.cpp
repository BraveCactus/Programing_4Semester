#include <iostream>
using namespace std;

//Пример специализации класса

template <typename T>
class MyClass{
public:
    void show(){
        cout << "General class" << endl;
    }
};

template <>
class MyClass<int>{
public:
    void show(){
        cout << "int class" << endl;
    }        
};

template <typename T> struct fwnode {
    T data_;
    fwnode<T> *next_;
};

int main(){
    //int y = 2; { int y = y; }
    //int x = 2; { int x[x]; }

    MyClass<double> obj1;
    obj1.show();

    MyClass<int> obj2;
    obj2.show();
}