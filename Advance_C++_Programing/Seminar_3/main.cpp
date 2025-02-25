//Умные указатели

template <typename T>
class MyUnique{
    T* ptr_ = nullptr;
    
    MyUnique(const MyUnique&) = delete;
    MyUnique& operator=(const MyUnique&) = delete;

public:

    MyUnique(T*&& new_ptr) : ptr_{new_ptr} {}
    // MyUnique operator= {const MyUnique} = delete;
    T* operator->(){
        return ptr_;
    }

    const T* operator->() const {
        return ptr_;
    }

};

class MyClass{
public:
    int a;
    MyClass{int  v} : a{v} {};
};

template <typename T, typename... Args>
MyUnique<T> MakeUnique(Args... args){
    return MakeUnique<T> {new T{args...}};
}

MyUnique<MyClass> gl_value = new MyClass;

auto other_ptr = new MyClass;

auto made_ptr = MakeUnique<MyClass>();

class OtherClass{
public:
    MyUnique<MyClass> value = new MyClass;
};

MyUnique<MyClass> value = new MyClass;
// MyUnique<int> value2 = value;

int main(){
    const OtherClass smth;
    auto val = smth.value->a;
    // smth.value->a = 7;
}

