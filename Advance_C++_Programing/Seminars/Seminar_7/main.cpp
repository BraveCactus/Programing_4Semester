#include <algorithm>
#include <iomanip>
#include <iostream>
#include <list>
#include <vector>

struct yes {
    char c;
};
struct no {
    char c;
    yes  cl;
};

// static_assert(sizeof(yes) != sizeof(no), "Выберите другие типы");

template <typename T>
yes test_have_sort(decltype(&T::sort));
template <typename T>
no test_have_sort(...);


template <typename T>
yes test_have_range(decltype(&T::begin), decltype(&T::end));
template <typename T>
no test_have_range(...);


template <typename T, bool have_sort, bool have_range>
struct fast_sort_helper;

template <typename T, bool arg>
struct fast_sort_helper<T, true, arg> {
    static void fast_sort(T& x) {
        std::cout << "x.sort();" << std::endl;
        x.sort();
    }
};

// template <typename T, bool arg>
// struct fast_sort_helper<T, true, arg> {
//     static void fast_sort(T& x) {
//         std::cout << "x.sort();" << std::endl;
//         x.sort();
//     }
// };

template <typename T>
struct fast_sort_helper<T, false, true> {
    static void fast_sort(T& x) {
        std::cout << "std::sort(x.begin(), x.end());" << std::endl;
        std::sort(x.begin(), x.end());
    }
};

template <typename T>
struct fast_sort_helper<T, false, false> {
    static void fast_sort(T& x) {
        static_assert(sizeof(T) < 0, "Нет способа сортировки");
    }
};

class A {
public:
    void sort() {};
};

class B {
public:
    void f();
    int* begin() {
        return a;
    }
    int* end() {
        return a + 10;
    }
    int a[10];
};


class C {
public:
    void sort() {};
    int* begin() {
        return a;
    }
    int* end() {
        return a + 10;
    }
    int a[10];
};

template <typename T>
void fast_sort(T& x) {
    fast_sort_helper<T, sizeof(test_have_sort<T>(NULL)) == sizeof(yes),
                     sizeof(test_have_range<T>(NULL, NULL)) ==
                         sizeof(yes)>::fast_sort(x);
}


int main() {
    A a;
    fast_sort(a);

    B b;
    fast_sort(b);

    C c;
    fast_sort(c);

    // std::list<int>     l{1, 2, 3, 4};
    // std::vector<float> v{4.6, 7.8, 8.0};

    // fast_sort(l);
    // fast_sort(v);
}
