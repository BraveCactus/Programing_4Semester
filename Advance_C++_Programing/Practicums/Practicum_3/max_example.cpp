#include <iostream>

//-> decltype((t > u)? t: u) не обязательно! Просто для примера, что такое можно делать (fucking python)
template <typename T, typename U>
auto max(T t, U u) -> decltype((t > u)? t: u){
    return (t > u)? t : u;
}

int main(int argc, char** argv){
    std::cout << max(3, 3.8) << std::endl;

    int x = 10;
    decltype(x) y = 20;
}