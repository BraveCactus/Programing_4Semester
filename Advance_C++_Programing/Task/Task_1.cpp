#include <iostream>
#include <string>
#include <vector>
#include <algorithm>  
#include <stdexcept> 
using namespace std; 

//To do:
// 1) Подумать над использованием ссылок или указетелей в idLists и namesLists
// 2) Не нравится что у User можно поменять ссылку на группу непосредственно через экземпляр этого класса

// Функция для красивого вывода списка
template <typename T>
void printVec(const std::vector<T> vec){
    for (int i = 0; i < vec.size(); i++)
    {
        std::cout << vec[i] << " ";
    }
    std::cout << std::endl;    
};

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

    std::vector<std::string>& getAllNames() const{
        return namesLists;
    }

    std::vector<int>& getAllID() const {
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

class User : public Entity {   
private:
    // std::string* groupLink = nullptr;  
    std::shared_ptr<std::string> groupLink; // Выгоднее так как происходит автоматическое освобождение памяти

public:
    User(): Entity() {}
    User(const std::string& userName) : Entity(userName){};
    User(const std::string& userName, int userID) : Entity(userName, userID) {}

    // Функция для задания ссылки группы, в которой состоит пользователь
    void setGroupLink(const std::string& link){
        groupLink = std::make_shared<std::string>(link);
    }

    // Функция для получения ссылки на группу, к которой принадлежит пользователь
    const std::string& getGroupLink() const {
        if (!groupLink) {
            throw std::runtime_error("Пользователь не принадлежит ни одной группе!");
        }
        return *groupLink;
    }
    // const std::string& getGroupLink() const {
    //     if(!groupLink){throw std::runtime_error("Данный пользователь не принадлежит ни одной группе!");}
    //     return *groupLink;
    // }



};

class Group : public Entity {
    std::vector<User*> usersList;
public:
    Group() : Entity() {}
    Group(const std::string& groupName) : Entity(groupName){}
    Group(const std::string& groupName, int groupID) : Entity(groupName, groupID) {}

    ~Group() override {
        usersList.clear();
    }

    // Добавляем пользователя в группу
    void addUser(User* user) {
        if (!user) {
            throw std::runtime_error("Попытка добавить nullptr!");
        }
        
        // Проверяем, что пользователя нет в этой группе
        auto it = std::find_if(usersList.begin(), usersList.end(),
            [user](User* u) { return u->getID() == user->getID(); });
            
        if (it != usersList.end()) {
            throw std::runtime_error("Пользователь уже в группе!");
        }
    
        usersList.push_back(user);
        user->setGroupLink(this->getName());
    }
    // void addUser(User* user) {        
    //     auto target_name = std::find(namesLists.begin(), namesLists.end(), user->getName());

    //     if(target_name == namesLists.end()){ throw std::runtime_error("Вы пытаетесь добавить несуществующего Userа!");}
    //     usersList.push_back(user);

    //     user->setGroupLink(this->name);
    // }

    // Удаляем пользователя из группы
    void deleteUser(User* user) {
        auto target_user = std::find(usersList.begin(), usersList.end(), user);
        if(target_user == usersList.end()){ throw std::runtime_error("Вы пытаетесь удалить несуществующего участника группы!");}
        usersList.erase(target_user);
    }
};

int main() {
    std::string name1 = "Serega";
    User u1(name1, 1); 

    std::string name2 = "Griga";
    User u2(name2, 2); 

    std::string name3 = "Miha";
    User u3(name3, 3);  

    std::vector<std::string>& allNames = u1.getAllNames(); 
    std::vector<int>& allID = u1.getAllID(); 
    
    printVec(allNames);   
    printVec(allID);

    std::string name4 = "Tipok1";
    User u4(name4); 

    printVec(allNames);   
    printVec(allID);

    std::string name5 = "Tipok2";
    User u5(name5); 

    printVec(allNames);   
    printVec(allID);
    
    return 0;
}