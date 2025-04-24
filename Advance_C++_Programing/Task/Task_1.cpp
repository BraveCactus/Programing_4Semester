#include <iostream>
#include <string>
#include <vector>
#include <algorithm>  
#include <stdexcept> 
using namespace std; 

template <typename T>
void printVec(const std::vector<T> vec){
    for (int i = 0; i < vec.size(); i++)
    {
        std::cout << vec[i] << " ";
    }
    std::cout << std::endl;    
};

class Entity {
protected:
    std::string name;     
    int id = 0;
    inline static std::vector<int> idLists;
    inline static std::vector<std::string> namesLists;

    void checkAndAddID(int value, std::vector<int>& list) {
        if (std::find(list.begin(), list.end(), value) != list.end()) { 
            throw std::runtime_error("Вы попытались присвоить ID, которое уже занято!"); 
        }
        list.push_back(value);
    }

    void checkAndAddName(const std::string& value, std::vector<std::string>& list) { 
        if (std::find(list.begin(), list.end(), value) != list.end()) { 
            throw std::runtime_error("Вы попытались присвоить имя, которое уже занято!"); 
        }
        list.push_back(value);
    }

public:
    std::vector<std::string> getAllNames(){
        return namesLists;
    }

    std::vector<int> getAllID(){
        return idLists;
    }

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

    std::string name2 = "Griga";
    User u2(name2, 2); 

    std::string name3 = "Miha";
    User u3(name3, 3);  

    std::vector<std::string> allNames = u1.getAllNames(); 
    
    printVec(allNames);   

    
    return 0;
}