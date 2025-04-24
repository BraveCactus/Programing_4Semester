#include <iostream>
#include <string>
#include <vector>
#include <algorithm>  
#include <stdexcept>  

class Entity {
protected:
    std::string name;     
    int id = 0;
    inline static std::vector<int> idLists;
    inline static std::vector<std::string> namesLists;

    void checkAndAddID(int value, std::vector<int>& list) {
        if (std::find(list.begin(), list.end(), value) != list.end()) { 
            throw std::runtime_error("это ID уже занято!"); 
        }
        list.push_back(value);
    }

    void checkAndAddName(const std::string& value, std::vector<std::string>& list) { 
        if (std::find(list.begin(), list.end(), value) != list.end()) { 
            throw std::runtime_error("это имя занято!"); 
        }
        list.push_back(value);
    }

public:
    Entity() : name(""), id(0) {}

    Entity(const std::string& initialName, int identifier) : name(initialName), id(identifier) {
        checkAndAddID(id, idLists);
        checkAndAddName(name, namesLists);
    }

    void setName(const std::string& newName) {
        if (newName == name) return;
        
        auto it_name = std::find(namesLists.begin(), namesLists.end(), name);
        if (it_name != namesLists.end()) {
            namesLists.erase(it_name);
        }
        
        checkAndAddName(newName, namesLists);
        name = newName;
    }
    
    void setID(int newID) {
        if (newID == id) return;
        
        auto it_id = std::find(idLists.begin(), idLists.end(), id);
        if (it_id != idLists.end()) {
            idLists.erase(it_id);
        }
        
        checkAndAddID(newID, idLists);
        id = newID;
    }

    std::string getName() const { return name; }
    int getID() const { return id; }

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
public:
    User() : Entity() {}
    User(const std::string& userName, int userID) : Entity(userName, userID) {}
};

class Group : public Entity {
    std::vector<User*> usersList;
public:
    Group() : Entity() {}
    Group(const std::string& groupName, int groupID) : Entity(groupName, groupID) {}

    ~Group() override {
        usersList.clear();
    }

    void addUser(User* user) {
        usersList.push_back(user);
    }
};

int main() {
    std::string name1 = "Serega";
    User u1(name1, 1); 
    
    return 0;
}