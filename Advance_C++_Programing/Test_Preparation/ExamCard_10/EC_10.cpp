// 10. Инкапсуляция. Определение
/*
Опр: Механизм объединения данных и методов, которые работают с этими данными, в единый объект, 
     и ограничение доступа к внутренним данным объекта.
*/

// Пример:

#include <iostream>

class Person {
private:
    std::string name;
    int age;
    
public:
    void setName(const std::string& newName) {
        if (!newName.empty()) {
            name = newName;
        }
    }
    
    std::string getName() const {
        return name;
    }
};