#include <iostream>
using namespace std;

//Задача: написать класс Logger, который будет запоминать кол-во экземпляров класса
//А также будет поддерживать операторы копирования, присваивания, конструктора и деструктора

class Logger{
private:
    inline static int counter = 0;
    const int id;
    
public:
    int value;

    Logger(): id(++counter){

    }

    //Конструктор
    Logger(int x): id(++counter), value(x){
        cout << "Выхов конструктора для объекта " << id << endl;
        cout << "Всего экземпляров класса: " << counter << endl;
        cout << endl;
    }

    //Оператор копирования
    Logger(const Logger& other): id(++counter), value(other.value){
        cout << "Вызов оператора копирования: " << other.id << "->" << id << endl;
        cout << "Всего экземпляров класса: " << counter << endl;
        cout << endl;
    }

    //Оператор присваивания
    Logger& operator=(const Logger& other){
        value = other.value;
        cout << "Выхов вызов оператора присваивания для объекта " << id << endl;
        cout << "Всего экземпляров класса: " << counter << endl;
        cout << endl;
        return *this;
    }

    //Деструктор
    ~Logger(){
        cout << "Вызов деструктора для объекта " << id << endl;
        counter -= 1;
        cout << "Всего экземпляров класса: " << counter << endl;
        cout << endl;
    }

    void print(){
        cout << endl;
        cout << "Значение объекта" << this->id << " = " << this->value << endl;
        cout << endl;
    }
};

//Написать класс bigLogger с полями класса Logger

class bigLogger{
private:
    const int id;
    inline static int counter;
    
public:
    Logger InnerLogger;
    bigLogger(Logger& x):id(++counter), InnerLogger(x){
        cout << "Конструктор bigLogger объекта номер: " << id << endl;
        cout << "Всего объектов bigLogger: " << counter << endl;
        cout << endl;
    }

    ~bigLogger(){
        counter -= 1;
        cout << "Вызов деструктора для объекта класса bigClass: " << id << endl;
        cout << "Всего объектов bigLogger: " << counter << endl;
        cout << endl;
    }

};

int main(){
    Logger c(1);
    bigLogger a(c);
    c.value = 2;

    cout << c.value << endl;
    cout << a.InnerLogger.value << endl;



}