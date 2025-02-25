


template <typename T>
class MyUnique {
    T* ptr_ = nullptr;


    MyUnique(const MyUnique&)            = delete;
    MyUnique& operator=(const MyUnique&) = delete;

public:
    MyUnique(T*&& new_ptr) : ptr_{new_ptr} {}
    T* operator->() const {
        return ptr_;
    }
    T& operator*() const {
        return *ptr_;
    }
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
    const OtherClass smth;
    auto             val = smth.value->a;
    smth.value->a        = 7;
    (*smth.value).a      = 5;
    (*gl_value).a        = 10;
    // value = value2;
    gl_value->a = 6;
}
