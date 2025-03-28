#include <algorithm>
#include <iomanip>
#include <iostream>
#include <list>
#include <vector>

template <class T, T v>
struct integral_constant {
  static constexpr T value = v;
};

struct true_type : public integral_constant<bool, true> {};

struct false_type : public integral_constant<bool, false> {};

template <class T>
struct same {
  using type = T;
};

template <bool V, class T = void>
struct enable_if;

template <class T>
struct enable_if<true, T> : public same<T> {};

template <bool V, class T = void>
using enable_if_t = typename enable_if<V, T>::type;

namespace detail {

template <typename T>
true_type test_have_sort(decltype(&T::sort));
template <typename T>
false_type test_have_sort(...);

template <class T>
struct __have_sort : public same<decltype(test_have_sort<T>(nullptr))> {};

template <typename T>
true_type test_have_range(decltype(&T::begin), decltype(&T::end));
template <typename T>
false_type test_have_range(...);

template <class T>
struct __have_range
    : public same<decltype(test_have_range<T>(nullptr, nullptr))> {};

};  // namespace detail

template <typename T, bool have_sort, bool have_range>
struct fast_sort_helper;

template <typename T, bool arg>
struct fast_sort_helper<T, true, arg> {
  static void fast_sort(T& x) {
    std::cout << "x.sort();" << std::endl;
    x.sort();
  }
};

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
  int* begin() { return a; }
  int* end() { return a + 10; }
  int a[10];
};

class C {
 public:
  void sort() {};
  int* begin() { return a; }
  int* end() { return a + 10; }
  int a[10];
};

template <typename T>
void fast_sort(T& x) {
  fast_sort_helper<T,
                   sizeof(detail::test_have_sort<T>(NULL)) == sizeof(true_type),
                   sizeof(detail::test_have_range<T>(NULL, NULL)) ==
                       sizeof(true_type)>::fast_sort(x);
}



int main() {
  A a;
  fast_sort(a);

  B b;
//   fast_sort(b);

  C c;
  fast_sort(c); 
}
