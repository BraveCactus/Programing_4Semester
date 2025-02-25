#include <iostream>
#include <string>
using namespace std;

// Кофемолка


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

int main(){

    CoffeeGrinder a;

    a.Start();
    
    return 0;
}