#include <iostream>
#include <string>
using namespace std;

// геттеры и сеттеры
// инкапсуляция

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


private:
    int x;
    int y;

public:
    int GetX(){
        return x;
    }

    int GetY(){
        return y;
    }

    void SetX(int valueX){
        x = valueX;
    }

    void SetY(int valueY){
        y = valueY;
    }

    void Print(){
        cout << x << " " << y << endl;
    }

};

int main(){
    Point a;

    a.SetX(2);
    a.SetY(6);

    a.Print();

    int value = a.GetX();
    cout << value << endl;
   
    return 0;
}