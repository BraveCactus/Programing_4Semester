#include <iostream>
#include <vector>
using namespace std;

template <typename T> 
struct list_t {
    struct node_t{
        node_t *next, *prev;
        T data_;      

        node_t(const T& value): data_(value), next(nullptr), prev(nullptr){}
    };
    node_t *top_, *back_;

    list_t(): top_(nullptr), back_(nullptr) {};
};

struct S{
    S() {cout << "default" << endl;};
    S(int key) {cout << "direct" << endl;};
};

struct Node1 {
    S key_;
    int val_;
    Node1 (int key, int val): key_(key), val_(val) {};
};

struct foo {
    foo() {cout << "1" << endl;};
    foo(const foo&) {cout <<"2" << endl;};
    ~foo() {cout << "3" << endl;};
};

void aa(const foo& f){ cout << "AA" << endl;};
foo bar() {foo loc_foo; return loc_foo;};

struct Matrix{
    Matrix(){cout << "created parent!" << endl;};
    virtual ~Matrix(){cout << "deleted parent!" << endl;};
    virtual void pow(int x) {cout << "slow pow" << endl;};
};

struct SparceMatrix: Matrix{
    SparceMatrix(){cout << "created child!" << endl;};
    ~SparceMatrix(){cout << "deleted child!" << endl;};
    void pow(long x){cout << "fast pow" << endl;};
};

struct Base{
    virtual void foo(int x = 14){
        cout << "Base: " << x << endl;
    };
};

struct Derived: Base {
    void foo(int x = 42) override{
        cout << "Derived: " << x << endl;
    }
};

struct Matrix2{ 
    virtual void pow(int x){ cout << " Matrix2 pow int" << endl;};
    virtual void pow(double x){ cout << " Matrix2 pow double" << endl;};
};

struct SpareMatrix2: Matrix2{
    using Matrix2::pow;
    void pow(int x) override {cout << "SpareMatrix2 pow int" << endl;};
};

int main(){    
    Matrix2 *m = new SpareMatrix2;

    m->pow(1.5);

    SpareMatrix2 d;

    d.pow(1.5);

    delete m;
}
    