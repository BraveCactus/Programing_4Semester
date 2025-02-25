#include <iostream>
#include <string>
using namespace std;

// Методы класса
// Функции класса

class Human{
public:
    int age;
    int weight;
    string name;

    void Print(){
        cout << "Имя: " << name << " Вес: " << weight << " Возраст: " << age << endl;
    }

};

class Point{
public:
    int x;
    int y;
    int z;

};

int main(){

    Human FirstHuman{30, 90, "Ivan"};
    FirstHuman.Print();
    
    return 0;
}