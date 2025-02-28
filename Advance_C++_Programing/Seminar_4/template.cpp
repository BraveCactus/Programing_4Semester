#include <iostream>
#include "template.h"
using namespace std;

extern void other();

template <typename T>
void func(T&& var){
    cout << "template " << var << endl;
}

void func(int&& var){
    cout << "simple " << var << endl;
}

int main(){
    const int a = 5;    
    
    func(5);
    func(move(a));
}
    