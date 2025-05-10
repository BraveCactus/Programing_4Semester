template <typename T>
class MyUnique {
    T* ptr_ = nullptr;  // Сырой указатель на объект

    // Запрещаем копирование
    MyUnique(const MyUnique&) = delete;
    MyUnique& operator=(const MyUnique&) = delete;

public:
    // Конструктор, принимающий rvalue-ссылку на указатель
    MyUnique(T*&& new_ptr) : ptr_{new_ptr} {}

    // Оператор доступа к членам (->)
    T* operator->() const { return ptr_; }

    // Оператор разыменования (*)
    T& operator*() const { return *ptr_; }
};
class MyClass {
public:
    int a;
    MyClass(int v) : a{v} {}
};

template <typename T, typename... Args>
MyUnique<T> MakeUnique(Args... args) {
    return MyUnique<T>{new T(args...)};
}


MyUnique<MyClass> gl_value = new MyClass(7);

auto other_ptr = new MyClass(5);
// MyUnique<MyClass> other_value = other_ptr;
// MyUnique<int> value2 = value;
auto made_ptr = MakeUnique<MyClass>(20);

class OtherClass {
public:
    MyUnique<MyClass> value = new MyClass(3);
};

int main() {
    const OtherClass smth;  // Создаёт MyUnique<MyClass> внутри

    // Доступ к полю 'a' через operator->
    auto val = smth.value->a;  // val = 3

    // Изменение поля 'a'
    smth.value->a = 7;         // Теперь a = 7
    (*smth.value).a = 5;       // Теперь a = 5 (через operator*)
    (*gl_value).a = 10;        // Изменение глобального объекта (a = 10)

    gl_value->a = 6;           // Изменение через operator->
}
