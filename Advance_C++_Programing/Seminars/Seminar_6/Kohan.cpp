#include <iostream>
#include <iomanip>
#include <vector>
using namespace std;

//Поменять input на контейнер
//Сравнить с обычной суммой

template<typename T> 
T myKohan(T sum_previous, T input, T& comp){     

    T y = input - comp;

    //Сумма
    T result_sum = sum_previous + y;

    //Пересчитываем компенсацию
    comp = (result_sum - sum_previous) - y;

    return result_sum;
}

int main(){
    float a = 10.0;
    float b = 0.3222123;
    float c = 0.0;    

    float sum = myKohan<float>(a, b, c);
    cout << setprecision(5) << sum << endl;
    cout << setprecision(5) << a + b << endl;

    vector<float> data = {10.0, 3.141592, 2.718374, 434.6, 0.3222123, 56,123};

    float kohanSum = 0;
    float simpleSum = 0;


    // for(auto x: data){
    //     simpleSum += x;

    // }
}