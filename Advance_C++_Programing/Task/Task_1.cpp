#include <iostream>
#include <string>
#include <vector>
#include <algorithm>  
#include <stdexcept> 
#include <memory>
using namespace std; 

//To do:
// 1) Подумать над использованием ссылок или указетелей в idLists и namesLists
// 2) Не нравится что у User можно поменять ссылку на группу непосредственно через экземпляр этого класса done

// Базовый класс, от которого наследуем User и Group
class Entity {
protected:
    std::string name;     
    int id = 0;
    inline static std::vector<int> idLists; // список id групп и участников
    inline static std::vector<std::string> namesLists; // список имен групп и участников

    // Проверка и добаление id 
    void checkAndAddID(int new_id, std::vector<int>& list) {
        if (std::find(list.begin(), list.end(), new_id) != list.end()) { 
            throw std::runtime_error("Вы попытались присвоить ID, которое уже занято!"); 
        }
        list.push_back(new_id);
    }

    // Проверка и добавление имени
    void checkAndAddName(const std::string& new_name, std::vector<std::string>& list) { 
        if (std::find(list.begin(), list.end(), new_name) != list.end()) { 
            throw std::runtime_error("Вы попытались присвоить имя, которое уже занято!"); 
        }
        list.push_back(new_name);
    }

public:
    
    // Конструктор по умолчанию
    Entity() : name("_"), id(0){
        
        for (int i = 0; i < idLists.size() + 1; i++)
        {
            if (find(idLists.begin(), idLists.end(), i) == idLists.end())
            {
                id = i;
                break;
            }            
        }
        checkAndAddID(id, idLists);      
        checkAndAddName(name, namesLists); 
        
    }

    // Конструктор без id
    Entity(const std::string& initialName) : name(initialName), id(0){
        
        for (int i = 0; i < idLists.size() + 1; i++)
        {
            if (find(idLists.begin(), idLists.end(), i) == idLists.end())
            {
                id = i;
                break;
            }            
        }      
        checkAndAddID(id, idLists);      
        checkAndAddName(name, namesLists);     
        
    }

    // Основной конструктор 
    Entity(const std::string& initialName, int identifier) : name(initialName), id(identifier) {
        checkAndAddID(id, idLists);
        checkAndAddName(name, namesLists);
    }

    // Сеттеры:
    // Задаем новое имя
    void setName(const std::string& newName) {
        if (newName == name) return;
        checkAndAddName(newName, namesLists);
        
        auto it_name = std::find(namesLists.begin(), namesLists.end(), name);
        if (it_name != namesLists.end()) {
            namesLists.erase(it_name);
        }        
        
        name = newName;
    }
    
    // Задаем новое id
    void setID(int newID) {
        if (newID == id) return;
        checkAndAddID(newID, idLists);
        
        auto it_id = std::find(idLists.begin(), idLists.end(), id);
        if (it_id != idLists.end()) {
            idLists.erase(it_id);
        }        
        
        id = newID;
    }

    // Геттеры:

    std::string getName() const { return name; }
    int getID() const { return id; }

    const std::vector<std::string>& getAllNames() const{
        return namesLists;
    }

    const std::vector<int>& getAllID() const {
        return idLists;
    }

    virtual ~Entity() {
        auto target_id = std::find(idLists.begin(), idLists.end(), id);
        if (target_id != idLists.end()) {
            idLists.erase(target_id);
        }

        auto target_name = std::find(namesLists.begin(), namesLists.end(), name);
        if (target_name != namesLists.end()) {
            namesLists.erase(target_name);
        }
    }

    
};

class Group; // Чтобы не было циклических зависимостей

class User : public Entity {   
private:     
    std::weak_ptr<Group> groupLink; // Выгоднее так как происходит автоматическое освобождение памяти + нет зацикленности

    // Функция для задания ссылки группы, в которой состоит пользователь
    friend class Group;
    void setGroup(std::shared_ptr<Group> group){
        groupLink = group;
    }

public:
    User(): Entity() {}
    User(const std::string& userName) : Entity(userName){};
    User(const std::string& userName, int userID) : Entity(userName, userID) {}    

    // Функция для получения ссылки на группу, к которой принадлежит пользователь
    const std::shared_ptr<Group> getGroupLink() const{               
        return groupLink.lock();
    }      
    
    void printInfo(); // Определим после Group, т.к. User еще не знает о group->getName
};

class Group : public Entity, public enable_shared_from_this<Group> {
    std::vector<std::shared_ptr<User>> usersList;
public:
    Group() : Entity() {}
    Group(const std::string& groupName) : Entity(groupName){}
    Group(const std::string& groupName, int groupID) : Entity(groupName, groupID) {}

    ~Group() override {
        usersList.clear();
    }

    // Добавление пользователя в группу
    void addUser(std::shared_ptr<User> user) {
        if (!user) {
            throw std::runtime_error("Попытка добавить nullptr!");
        }
        
        // Проверка, что пользователя нет в группе
        auto it = std::find_if(usersList.begin(), usersList.end(),
            [user](const auto& u) { return u->getID() == user->getID(); });
            
        if (it != usersList.end()) {
            throw std::runtime_error("Пользователь уже в группе!");
        }
    
        usersList.push_back(user);
        user->setGroup(shared_from_this()); // Устанавливаем ссылку на группу
    }

    // Удаление пользователя из группы
    void removeUser(std::shared_ptr<User> user) {
        // Проверка на наличие пользователя в группе
        auto it = std::find_if(usersList.begin(), usersList.end(),
            [user](const auto& u) { return u->getID() == user->getID(); });
            
        if (it == usersList.end()) {
            throw std::runtime_error("Пользователь не найден в группе!");
        }
        
        (*it)->setGroup(nullptr); // Удаляем ссылку на группу
        usersList.erase(it);
    }

    // Получение списка пользователей
    const std::vector<std::shared_ptr<User>>& getUsers() const {
        return usersList;
    }

    void printInfo(){
        std::cout << "Имя: " << name << " ID: " << id << std::endl;
        std::cout << "Список участников: ";
        if (usersList.empty()) {
            std::cout << "(нет участников)" << std::endl;
            return;
        }
        for (const auto& user : usersList) {
            std::cout << user->getName() << " (ID: " << user->getID() << "), ";
        }
        std::cout << std::endl;
    }
        
};

// Теперь определяем User::printInfo(), так как Group уже полностью определён
void User::printInfo() {
    std::cout << "Имя: " << name << " ID: " << id << std::endl;
    if (auto group = groupLink.lock()) {
        std::cout << "Имя группы: " << group->getName() 
                  << "Адрес группы: " << group.get() << std::endl;
    } else {
        std::cout << "Группа: nullptr" << std::endl;
    }
}


int main() {

    auto u5 = std::make_shared<User>("Dimon");
    u5->printInfo();
    auto u6 = std::make_shared<User>("Ilyha");
    u6->printInfo();
    auto u7 = std::make_shared<User>("Nekit-Pityx");
    u7->printInfo();

    auto g1 = std::make_shared<Group>("KGKP");
    g1->printInfo();

    g1->addUser(u5);    

    g1->addUser(u6);    

    g1->addUser(u7);
    
    g1->printInfo();

   
    return 0;
}