#include <iostream>
#include <string>
#include <vector>
#include <algorithm>  
#include <stdexcept> 
#include <memory>
#include <map>
#include <sstream>
using namespace std; 

//To do:
// 1) Подумать над использованием ссылок или указетелей в idLists и namesLists
// 2) Не нравится что у User можно поменять ссылку на группу непосредственно через экземпляр этого класса done
// 3) Добавить доп инфу для классов done
// 4) Проблемы с занесением доп информации

// Базовый класс, от которого наследуем User и Group
class Entity {
protected:
    std::string name;     
    int id = 0;
    std::string addInfo;
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
    Entity() : name("_"), id(0), addInfo("_"){
        
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
    Entity(const std::string& initialName) : name(initialName), id(0), addInfo("_"){
        
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
    Entity(const std::string& initialName, int identifier, const std::string& info = "_") : name(initialName), id(identifier), addInfo(info) {
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

    void setInfo(const std::string& info){
        addInfo = info;
    }

    // Геттеры:

    std::string getName() const { return name; }
    int getID() const { return id; }
    std::string getInfo() const { return addInfo; }

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
    friend class UserGroupManager;
    void setGroup(std::shared_ptr<Group> group){
        groupLink = group;
    }

public:
    User(): Entity() {}
    User(const std::string& userName) : Entity(userName){};
    User(const std::string& userName, int userID, const std::string& userInfo = "_") : Entity(userName, userID, userInfo) {}    

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
    Group(const std::string& groupName, int groupID, const std::string& groupInfo = "_") : Entity(groupName, groupID, groupInfo) {}

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
        std::cout << "Имя: " << name << " ID: " << id << " Доп. информация: " << addInfo << std::endl;
        std::cout << "Список участников: ";
        if (usersList.empty()) {
            std::cout << "(нет участников)" << std::endl;
            return;
        }
        for (const auto& user : usersList) {
            std::cout << user->getName() << " (ID: " << user->getID() << ", Доп. информация: " << user->getInfo() << "), ";
        }
        std::cout << std::endl;
    }
};

// Теперь определяем User::printInfo(), так как Group уже полностью определён
void User::printInfo() {
    std::cout << "Имя: " << name << " ID: " << id << " Доп. информация: " << addInfo << std::endl;
    if (auto group = groupLink.lock()) {
        std::cout << "Имя группы: " << group->getName() 
                  << " Адрес группы: " << group.get() 
                  << " Доп. информация группы: " << group->getInfo() << std::endl;
    } else {
        std::cout << "Группа: nullptr" << std::endl;
    }
}


class UserGroupManager {
private:
    map<int, shared_ptr<User>> users;
    map<int, shared_ptr<Group>> groups;

public:
    // Создание пользователя
    void createUser(int userId, const string& username, const string& userInfo = "_") {
        if (users.find(userId) != users.end()) {
            cout << "Ошибка: пользователь с ID " << userId << " уже существует" << endl;
            return;
        }
        try {
            users[userId] = make_shared<User>(username, userId, userInfo);
            cout << "Пользователь " << username << " (ID: " << userId << ", Доп. информация: " << userInfo << ") успешно создан" << endl;
        } catch (const exception& e) {
            cout << "Ошибка создания пользователя: " << e.what() << endl;
        }
    }

    // Удаление пользователя
    void deleteUser(int userId) {
        auto it = users.find(userId);
        if (it == users.end()) {
            cout << "Ошибка: пользователь с ID " << userId << " не найден" << endl;
            return;
        }
        
        // Удаляем пользователя из всех групп
        if (auto group = it->second->getGroupLink()) {
            group->removeUser(it->second);
        }
        
        users.erase(it);
        cout << "Пользователь с ID " << userId << " успешно удален" << endl;
    }

    // Вывод всех пользователей
    void allUsers() const {
        if (users.empty()) {
            cout << "Нет ни одного пользователя" << endl;
            return;
        }
        
        cout << "Список всех пользователей:" << endl;
        for (const auto& [id, user] : users) {
            user->printInfo();
            cout << "-------------------" << endl;
        }
    }

    // Получение информации о пользователе
    void getUser(int userId) const {
        auto it = users.find(userId);
        if (it == users.end()) {
            cout << "Ошибка: Пользователя с ID " << userId << " не найден" << endl;
            return;
        }
        
        cout << "Информация о пользователе:" << endl;
        it->second->printInfo();
    }

    // Создание группы
    void createGroup(int groupId, const string& groupName, const string& groupInfo = "_") {
        if (groups.find(groupId) != groups.end()) {
            cout << "Ошибка: Группа с ID " << groupId << " уже существует" << endl;
            return;
        }
        try {
            groups[groupId] = make_shared<Group>(groupName, groupId, groupInfo);
            cout << "Группа " << groupName << " (ID: " << groupId << ", Доп. информация: " << groupInfo << ") успешно создана" << endl;
        } catch (const exception& e) {
            cout << "Ошибка создания группы: " << e.what() << endl;
        }
    }

    // Удаление группы
    void deleteGroup(int groupId) {
        auto it = groups.find(groupId);
        if (it == groups.end()) {
            cout << "Ошибка: Группа с ID " << groupId << " не найдена" << endl;
            return;
        }
        
        // Удаляем ссылки на группу у всех пользователей
        for (auto& user : it->second->getUsers()) {
            user->setGroup(nullptr);
        }
        
        groups.erase(it);
        cout << "Группа с ID " << groupId << " успешно удалена" << endl;
    }

    // Вывод всех групп
    void allGroups() const {
        if (groups.empty()) {
            cout << "Нет никаких групп" << endl;
            return;
        }
        
        cout << "Список всех групп:" << endl;
        for (const auto& [id, group] : groups) {
            group->printInfo();
            cout << "-------------------" << endl;
        }
    }

    // Получение информации о группе
    void getGroup(int groupId) const {
        auto it = groups.find(groupId);
        if (it == groups.end()) {
            cout << "Ошибка: Группа с ID " << groupId << " не найдена" << endl;
            return;
        }
        
        cout << "Информация о группе:" << endl;
        it->second->printInfo();
    }

    // Добавление пользователя в группу
    void addUserToGroup(int userId, int groupId) {
        auto userIt = users.find(userId);
        auto groupIt = groups.find(groupId);
        
        if (userIt == users.end()) {
            cout << "Ошибка: Пользователь с ID " << userId << " не найден" << endl;
            return;
        }
        if (groupIt == groups.end()) {
            cout << "Ошибка: Группа с ID " << groupId << " не найдена" << endl;
            return;
        }
        
        try {
            groupIt->second->addUser(userIt->second);
            cout << "Пользователь " << userIt->second->getName() << " (ID: " << userId 
                 << ") добавлен в группу " << groupIt->second->getName() 
                 << " (ID: " << groupId << ")" << endl;
        } catch (const exception& e) {
            cout << "Ошибка: " << e.what() << endl;
        }
    }

    // Удаление пользователя из группы
    void removeUserFromGroup(int userId, int groupId) {
        auto userIt = users.find(userId);
        auto groupIt = groups.find(groupId);
        
        if (userIt == users.end()) {
            cout << "Ошибка: Пользователь с ID " << userId << " не найден" << endl;
            return;
        }
        if (groupIt == groups.end()) {
            cout << "Ошибка: Группа с ID " << groupId << " не найдена" << endl;
            return;
        }
        
        try {
            groupIt->second->removeUser(userIt->second);
            cout << "Пользователь " << userIt->second->getName() << " (ID: " << userId 
                 << ") удален из группы " << groupIt->second->getName() 
                 << " (ID: " << groupId << ")" << endl;
        } catch (const exception& e) {
            cout << "Ошибка: " << e.what() << endl;
        }
    }

    // Установка дополнительной информации для пользователя
    void setUserInfo(int userId, const string& info) {
        auto it = users.find(userId);
        if (it == users.end()) {
            cout << "Ошибка: Пользователь с ID " << userId << " не найден" << endl;
            return;
        }
        it->second->setInfo(info);
        cout << "Доп. информация для пользователя " << it->second->getName() 
             << " (ID: " << userId << ") установлена: " << info << endl;
    }

    // Установка дополнительной информации для группы
    void setGroupInfo(int groupId, const string& info) {
        auto it = groups.find(groupId);
        if (it == groups.end()) {
            cout << "Ошибка: Группа с ID " << groupId << " не найдена" << endl;
            return;
        }
        it->second->setInfo(info);
        cout << "Доп. информация для группы " << it->second->getName() 
             << " (ID: " << groupId << ") установлена: " << info << endl;
    }
};

void printHelp() {
    cout << "Доступные команды:" << endl;
    cout << "  createUser <userId> <username> [доп.инфо] - Создать нового пользователя" << endl;
    cout << "  deleteUser <userId> - Удалить пользователя" << endl;
    cout << "  allUsers - Показать всех пользователей" << endl;
    cout << "  getUser <userId> - Показать информацию о пользователе" << endl;
    cout << "  createGroup <groupId> <groupName> [доп.инфо] - Создать новую группу" << endl;
    cout << "  deleteGroup <groupId> - Удалить группу" << endl;
    cout << "  allGroups - Показать все группы" << endl;
    cout << "  getGroup <groupId> - Показать информацию о группе" << endl;
    cout << "  addUserToGroup <userId> <groupId> - Добавить пользователя в группу" << endl;
    cout << "  removeUserFromGroup <userId> <groupId> - Удалить пользователя из группы" << endl;
    cout << "  setUserInfo <userId> <инфо> - Установить доп. информацию для пользователя" << endl;
    cout << "  setGroupInfo <groupId> <инфо> - Установить доп. информацию для группы" << endl;
    cout << "  help - Показать эту справку" << endl;
    cout << "  exit - Выйти из программы" << endl;
}

void processCommand(const string& command, UserGroupManager& manager) {
    istringstream iss(command);
    string cmd;
    iss >> cmd;

    if (cmd == "createUser") {
        int userId;
        string username, info = "_";
        if (iss >> userId >> username) {
            getline(iss, info); // Читаем оставшуюся часть строки как доп. информацию
            if (!info.empty() && info[0] == ' ') {
                info = info.substr(1); // Удаляем первый пробел
            }
            manager.createUser(userId, username, info);
        } else {
            cout << "Ошибка: Неверные аргументы для createUser. Использование: createUser <userId> <username> [доп.инфо]" << endl;
        }
    }
    else if (cmd == "deleteUser") {
        int userId;
        if (iss >> userId) {
            manager.deleteUser(userId);
        } else {
            cout << "Ошибка: Неверные аргументы для deleteUser. Использование: deleteUser <userId>" << endl;
        }
    }
    else if (cmd == "allUsers") {
        manager.allUsers();
    }
    else if (cmd == "getUser") {
        int userId;
        if (iss >> userId) {
            manager.getUser(userId);
        } else {
            cout << "Ошибка: Неверные аргументы для getUser. Использование: getUser <userId>" << endl;
        }
    }
    else if (cmd == "createGroup") {
        int groupId;
        string groupName, info = "_";
        if (iss >> groupId >> groupName) {
            getline(iss, info);
            if (!info.empty() && info[0] == ' ') {
                info = info.substr(1);
            }
            manager.createGroup(groupId, groupName, info);
        } else {
            cout << "Ошибка: Неверные аргументы для createGroup. Использование: createGroup <groupId> <groupName> [доп.инфо]" << endl;
        }
    }
    else if (cmd == "deleteGroup") {
        int groupId;
        if (iss >> groupId) {
            manager.deleteGroup(groupId);
        } else {
            cout << "Ошибка: Неверные аргументы для deleteGroup. Использование: deleteGroup <groupId>" << endl;
        }
    }
    else if (cmd == "allGroups") {
        manager.allGroups();
    }
    else if (cmd == "getGroup") {
        int groupId;
        if (iss >> groupId) {
            manager.getGroup(groupId);
        } else {
            cout << "Ошибка: Неверные аргументы для getGroup. Использование: getGroup <groupId>" << endl;
        }
    }
    else if (cmd == "addUserToGroup") {
        int userId, groupId;
        if (iss >> userId >> groupId) {
            manager.addUserToGroup(userId, groupId);
        } else {
            cout << "Ошибка: Неверные аргументы для addUserToGroup. Использование: addUserToGroup <userId> <groupId>" << endl;
        }
    }
    else if (cmd == "removeUserFromGroup") {
        int userId, groupId;
        if (iss >> userId >> groupId) {
            manager.removeUserFromGroup(userId, groupId);
        } else {
            cout << "Ошибка: Неверные аргументы для removeUserFromGroup. Использование: removeUserFromGroup <userId> <groupId>" << endl;
        }
    }
    else if (cmd == "setUserInfo") {
        int userId;
        string info;
        if (iss >> userId) {
            getline(iss, info);
            if (!info.empty() && info[0] == ' ') {
                info = info.substr(1);
            }
            manager.setUserInfo(userId, info);
        } else {
            cout << "Ошибка: Неверные аргументы для setUserInfo. Использование: setUserInfo <userId> <инфо>" << endl;
        }
    }
    else if (cmd == "setGroupInfo") {
        int groupId;
        string info;
        if (iss >> groupId) {
            getline(iss, info);
            if (!info.empty() && info[0] == ' ') {
                info = info.substr(1);
            }
            manager.setGroupInfo(groupId, info);
        } else {
            cout << "Ошибка: Неверные аргументы для setGroupInfo. Использование: setGroupInfo <groupId> <инфо>" << endl;
        }
    }
    else if (cmd == "help") {
        printHelp();
    }
    else if (cmd == "exit") {
        exit(0);
    }
    else {
        cout << "Ошибка: Неизвестная команда. Введите 'help' для списка команд." << endl;
    }
}

int main() {
    // UserGroupManager manager;
    
    // manager.createUser(1, "Dimon", "Любит программирование");
    // manager.createUser(2, "Ilyha", "Отличник");
    // manager.createUser(3, "Nekit-Pityx", "Весельчак");
    
    // manager.createGroup(4, "KGKP", "Группа программистов");
    
    // manager.addUserToGroup(1, 4);
    // manager.addUserToGroup(2, 4);
    // manager.addUserToGroup(3, 4);
    
    // manager.allUsers();
    // manager.allGroups();
    
    // manager.setUserInfo(2, "Очень умный");
    // manager.setGroupInfo(4, "Лучшая группа");
    
    // manager.getUser(2);
    // manager.getGroup(4);

    UserGroupManager manager;
    string command;

    cout << "Система управления пользователями и группами" << endl;
    cout << "Введите 'help' для списка команд" << endl;

    while (true) {
        cout << "> ";
        getline(cin, command);
        if (!command.empty()) {
            processCommand(command, manager);
        }
    }

    return 0;
}