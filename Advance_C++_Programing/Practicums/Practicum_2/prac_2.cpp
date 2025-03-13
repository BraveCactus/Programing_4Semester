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