#include <iostream>
#include <iomanip>
using namespace std;

class MyClass{
    int a = 8;
public:
    MyClass(int value):a(value){};
    MyClass(){};
    bool testSmth(){
        return a > 0;
    };
} g; // экземпляр класса 

//Куммулятивное суммирование
inline float kahanSummation(float sum_previous, float input,
    float& accumulator) {
std::cout << std::setw(10) << std::setprecision(19) << sum_previous << input
<< accumulator << std::endl;
const float y = input - accumulator;
std::cout << std::setprecision(19) << "y = " << y << std::endl;
const float t = sum_previous + y;
std::cout << std::setprecision(19) << "t = " << t << std::endl;


accumulator = (t - sum_previous) - y;

std::cout << std::setprecision(19) << "t - sum = " << (t - sum_previous)
<< std::endl;
std::cout << std::setprecision(19) << "acc = " << accumulator << std::endl;

return t;
}


int main(){

    bool b1 = true;
    bool b2 = false;

    int c = 8;

    MyClass* ptr = new MyClass;

    ptr = nullptr;

    // cout << g.testSmth() << endl;

    ptr->testSmth();

    // if (ptr && ptr->testSmth()){ cout << "Ok!" << endl;}


}