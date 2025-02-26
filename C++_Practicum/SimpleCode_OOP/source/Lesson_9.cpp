#include <iostream>
#include <string>
using namespace std;

// деструкторы

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

    Point(){
        x = 0;
        y = 0;
    }

    Point(int valueX, int valueY)
    {
        x = valueX;
        y = valueY;
    }


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

class CoffeeGrinder{
private:

    bool CheckVoltage(){

        return true;
    }

public:
    void Start()
    {
        if(CheckVoltage()){
            cout << "Vjuuuhhh"  << endl;
        }
        else 
        {
            cout << "beep beep" << endl;
        }
        
    }
};

class MyClass{
private:
    int* data;
public:
    MyClass(int size)
    {
        data = new int[size];
        for(int i = 0; i != size; ++i)
        {
            data[i] = i;
        }
        cout << "Объект: "<< data << " Constructor!" << endl;
    }

    ~MyClass()
    {
        delete[] data;
        cout << "Объект: "<< data << " Destructor!" << endl;        
    }
};

void foo(){
    cout << "Начало выполнения foo" << endl;
    MyClass a(1);
    cout << "Конец выполнения foo" << endl;
}

int main(){

    foo();
    return 0;
}