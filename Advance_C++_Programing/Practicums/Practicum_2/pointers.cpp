/*
Опр: std::unique_ptr (уникальный указатель)
     1) Эксклюзивное владение ресурсом
     2) Невозможно копировать (только перемещение)
     3) Автоматически удаляет объект при выходе из области видимости
     4) Малые накладные расходы
Опр: std::shared_ptr (разделяемый указатель)
     1) Разделяемое владение ресурсом
     2) Использует подсчет ссылок
     3) Удаляет объект, когда счетчик ссылок достигает нуля
     4) Небольшие дополнительные накладные расходы
Опр: std::weak_ptr (слабый указатель)
     1) Не увеличивает счетчик ссылок shared_ptr
     2) Используется для решения проблемы циклических ссылок
     3) Для доступа к объекту нужно преобразовать в shared_ptr
 */
#include <iostream>
#include <memory>
using namespace std;

class MyClass{
public:
    MyClass(){cout << "Сreated!" << endl;};
    ~MyClass(){cout << "Deleted!" << endl;};

};

void foo(){
    
    unique_ptr<MyClass> m = make_unique<MyClass>();
    cout << "work" << endl;    
}

int main(){

    //Способы создания std::unique_ptr
    unique_ptr<MyClass> unique_p1(new MyClass);
    unique_ptr<MyClass> unique_p2 = make_unique<MyClass>();

    //Способы создания std::shared_ptr
    shared_ptr<MyClass> shared_p1(new MyClass);
    shared_ptr<MyClass> shared_p2(new MyClass);

    //Способы создания std::weak_ptr
    shared_ptr<MyClass> shared_p = make_shared<MyClass>();
    weak_ptr<MyClass> weak_p1(shared_p);
    weak_ptr<MyClass> weak_p2(weak_p1);

    // foo();

    shared_ptr<MyClass> p = make_shared<MyClass>();
    shared_ptr<MyClass> p_copy = p;
    cout << p.use_count() << endl;

    
    //Пример использования std::unique_ptr

    unique_ptr<int> ptr1(new int(10));
    auto ptr2 = make_unique<int>(10);
    auto ptr3 = std::move(ptr2);  // ptr теперь nullptr

    //Пример использования std::shared_ptr
    // Создание shared_ptr
    std::shared_ptr<int> ptr4(new int(10));
    
    // Копирование увеличивает счетчик ссылок
    auto ptr5 = ptr4;
    
    // Оба указателя работают с одним объектом
    *ptr1 = 20;
    cout << *ptr2;  // Выведет 20
    ptr4.use_count();  // Возвращает количество shared_ptr, владеющих объекто
    
    // Память освободится, когда оба указателя выйдут из области видимости\

    //Пример использования std::weak_ptr
    std::shared_ptr<int> shared(new int(10));
    std::weak_ptr<int> weak = shared;
    
    // Проверка, жив ли еще объект
    if (auto temp = weak.lock()) {
        // Используем temp (shared_ptr)
        *temp = 20;
    }
    // temp уничтожается здесь, счетчик ссылок уменьшается
    return 0;
}