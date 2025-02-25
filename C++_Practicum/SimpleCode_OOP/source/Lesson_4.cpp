#include <iostream>
#include <string>
using namespace std;

// Модификаторы доступа

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
    int z;

    void Print(){
        cout << "x: " << x << " y: " << " z: " << z << endl;
    }
private:
    int x;
protected:
    int y;   

};

int main(){

   Point a;
   a.z = 9;

   a.Print();
    
    return 0;
}