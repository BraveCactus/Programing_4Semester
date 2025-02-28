#include <iostream>
using namespace std;

template <typename T> T do_power(T x, T acc, unsigned n){
    while(n > 0){
        if ((n & 0x1) == 0x1) {acc *= x; n -= 1;}
        else {x *= x; n/=2;}
    }
    return acc;
}

unsigned power(unsigned x, unsigned n){
    
    if((x < 2u) || (n == 1u)) return x;    
        return do_power<unsigned>(x, 1u, n);
}

int main(){

    int a = 2;
    int n = 10;

    int result = power(a, n);

    cout << result << endl;

}