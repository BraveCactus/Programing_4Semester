/*
Опр: Хинты - (от англ. hint — подсказка) обычно относится к дополнительной информации, 
     которую разработчик предоставляет компилятору или стандартной библиотеке для оптимизации работы кода. 
     Хинты не меняют семантику программы, но могут влиять на её производительность.
Опр: decltype(name) - вывод точного типа name

*/

#include <iostream>
#include <vector>
#include <string>

template <typename T>
void print(T x) {
    std::cout << x << std::endl;
}

template <>
void print<int>(int x) {
    std::cout << "blablabla" << std::endl;
}

// Специализация для const char*
template <>
void print<const char*>(const char* x) {
    std::cout << x << std::endl;
}

class Builder {
public:
    std::string create() {
        return "create!";
    }
};

template<typename T>
auto process(const T& builder) -> decltype(builder.create()) {
    return builder.create();  // Добавлен return
}

int main() {
    double a = 5.5;
    print<double>(a);  // Выведет 5.5
    print<int>(a);     // Выведет "blablabla"

    int x = 90;

    // Пример хинтов (C++20)
    if (x > 0) [[likely]] {
        print("Более вероятный случай");
    } else [[unlikely]] {
        print("Менее вероятный случай");
    }

    std::vector<int> data = {1, 3, 3};
    int target = 3;

    for (int i = 0; i < data.size(); ++i) {
        if (data[i] == target) [[likely]] {
            print("Частый случай");
        }
    }

    Builder b;
    // std::string result = process(b);  // Почему то ошибка
    // print(result);
}