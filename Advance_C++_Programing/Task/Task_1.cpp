#include <iostream>
#include <string>
#include <vector>
// using namespace std;

// To do
// 1) Добавить конструкторы по умолчанию 
// 2) Добавить консольные утилиты
// 3) Доработать сеттеры: добавить проверку на уникальность, обновлять списки

// Базовый класс, от него будем наследовать User и Group
class Entity{
    protected:
        std::string name;     
        int id = 0;
        inline static std::vector<int> idLists; // список идентификаторов (общий для всего класса!)
        inline static std::vector<std::string> namesLists; // список имен (общий для всего класса!)        

        // Добавление ID
        void checkAndAddID(int value, std::vector<int>& list){
            if(find(list.begin(), list.end(), value) != list.end()){ throw std::runtime_error("это ID уже занято!"); }
            list.push_back(value);
        }

        // Обновление ID
        void updateListID(int newValue, std::vector<int>& list){
            if (newValue == id) {return;}
            checkAndAddID(newValue, list);
            auto target_id = find(list.begin(), list.end(), this->id);
            if(target_id != list.end()) { throw std::runtime_error("ошибка удаления старого ID!"); }
            list.erase(target_id);
        }

        // Добаление имени
        void checkAndAddName(std::string value, std::vector<std::string>& list){
            if(find(list.begin(), list.end(), value) != list.end()){ throw std::runtime_error("это имя занято!"); }
            list.push_back(value);
        }

        // Обновление имени
        void updateListName(std::string newValue, std::vector<std::string>& list){
            if (newValue == name) {return;}
            checkAndAddName(newValue, list);
            auto target_id = find(list.begin(), list.end(), this->name);
            if(target_id != list.end()) { throw std::runtime_error("ошибка удаления старого имени!"); }
            list.erase(target_id);
        }

    public:
        // Конструктор по умолчанию
        Entity(): name(""), id(0){}

        // Основной конструктор
        Entity(std::string initialName, int identifier): name(initialName), id(identifier){
            // Проверяем id на уникальность
            if(find(idLists.begin(), idLists.end(), id) != idLists.end()) { throw std::runtime_error("ID already exists!"); }
            idLists.push_back(identifier);

            // Проверяем name на уникальность
            if(find(namesLists.begin(), namesLists.end(), initialName) != namesLists.end()) { throw std::runtime_error("This name already exists!"); }
            namesLists.push_back(initialName);
        }        

        // Сеттеры
        void setName(const std::string& newName) {
            updateListName(newName, namesLists);
            name = newName;
        }   
        
        void setID(const int newID){
            updateListID(newID, idLists);
            id = newID;
        }

        // Геттеры
        std::string getName(){
            return name;
        }

        int getID(){
            return id;
        }

        // Деструктор
        virtual ~Entity(){
            // Удаляем id удаленной сущности из idLists
            auto target_id = std::find(idLists.begin(), idLists.end(), id);
            if (target_id != idLists.end()) {
                idLists.erase(target_id);
            }

            // Удаляем name удаленной сущности из namesLists
            auto target_name = std::find(namesLists.begin(), namesLists.end(), name);
            if (target_name != namesLists.end()) {
                namesLists.erase(target_name);
            }
        }    
};

// Класс User
class User: public Entity{     
    public:
        User(std::string userName, int userID): Entity(userName, userID){}
};


// Класс Group
class Group: public Entity{
    private:
        std::vector<User*> usersList; // Список участников
    public:
        Group(std::string groupName, int groupID): Entity(groupName, groupID){}

        ~Group() override {
            // Подумать о том что делать с участниками после удаления группы
            usersList.clear();
        }


};

int main(int argc, char* argv[]){
    std::string name1 = "Serega";
    User u1(name1, 1);


}