// 9. Инварианты класса. Определение.
/*
    Опр: Условия, которые должны выполняться для всех объектов класса на протяжении всего времени их существования.
*/

// Пример:

#include <iostream>

class Account {
    double balance;
public:
    Account() : balance(0) {}
    
    void deposit(double amount) {
        if (amount <= 0) throw std::invalid_argument("Amount must be positive");
        balance += amount;
    }
    
    void withdraw(double amount) {
        if (amount <= 0) throw std::invalid_argument("Amount must be positive");
        if (amount > balance) throw std::runtime_error("Insufficient funds");
        balance -= amount;
    }
    
    // Инвариант: balance >= 0
};