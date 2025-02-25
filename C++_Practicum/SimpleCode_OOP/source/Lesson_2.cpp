#include <iostream>
#include <string>
using namespace std;

// Что такое класс?
// Что такое объект класса?

class Human{
public:
    int age;
    string name;

};

class Point{
public:
    int x;
    int y;
    int z;

};

int main(){
    Human Matvey{19, "Matvey"};
    cout << Matvey.age << endl;    
    return 0;
}