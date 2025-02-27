#include <iostream>
#include <memory>
using namespace std;

//Умные указатели
//std::unique_ptr
//std::shared_ptr
//std::weak_ptr

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
    
    return 0;
}