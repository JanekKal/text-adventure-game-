#include <iostream>
#include <map>
#include <vector>
#include <algorithm>
#include <fstream>
#include "json.hpp"
#include <cstdlib>
#include <ctime>
#include <random>

using json = nlohmann::json;

class AdvGame {
public:
    AdvGame(json data) {
        // Initialize rooms
        for (int i = 0; i < data["rooms"].size(); i++) {
            allRooms.push_back({
                data["rooms"][i]["id"], 
                data["rooms"][i]["desc"],
                data["rooms"][i]["exits"]
            });
        }

        // Initialize objects
        for (int i = 0; i < data["objects"].size(); i++) {
            std::string location = data["objects"][i]["initialroom"];
            auto roomIt = std::find_if(allRooms.begin(), allRooms.end(), [&location, this](const Room& room) {
                return location == room.id;
            });

            if (roomIt != allRooms.end()) {
                allObjects.push_back({
                    data["objects"][i]["id"],
                    data["objects"][i]["desc"],
                    *roomIt,
                    false
                });
            }
            else {
                 allObjects.push_back({
                    data["objects"][i]["id"],
                    data["objects"][i]["desc"]
                });
            }
        }
        
        // Initialize treasure
        for (int i = 0; i < data["treasure"].size(); i++) {
            std::string initialRoom = data["treasure"][i]["initialroom"];
            auto roomIt = std::find_if(allRooms.begin(), allRooms.end(), [&initialRoom, this](const Room& room) {
                return initialRoom == room.id;
            });

            std::vector<Object> objects;
            for (auto& objectId : data["treasure"][i]["contents"]) {
                auto objectIt = std::find_if(allObjects.begin(), allObjects.end(), [&objectId, this](const Object& object) {
                    return objectId == object.id;
                });
                
                objects.push_back(*objectIt);
                (*objectIt).room = *roomIt;
                (*objectIt).hidden = true;
            }

            allTreasure.push_back({
                data["treasure"][i]["id"],
                data["treasure"][i]["desc"],
                *roomIt,
                data["treasure"][i]["openwith"],
                objects
            });

        }
        
        // Initialize food
        for (int i = 0; i < data["food"].size(); i++) {
            std::string initialRoom = data["food"][i]["initialroom"];
            auto roomIt = std::find_if(allRooms.begin(), allRooms.end(), [&initialRoom, this](const Room& room) {
                return initialRoom == room.id;
            });

            allFood.push_back({
                data["food"][i]["id"],
                data["food"][i]["desc"],
                *roomIt,
                data["food"][i]["regeneration"]
            });
        }
        
        // Initialize enemies
        for (int i = 0; i < data["enemies"].size(); i++) {
            std::string initialRoom = data["enemies"][i]["initialroom"];
            auto roomIt = std::find_if(allRooms.begin(), allRooms.end(), [&initialRoom, this](const Room& room) {
                return initialRoom == room.id;
            });

            allEnemies.push_back({
                data["enemies"][i]["id"],
                data["enemies"][i]["desc"],
                data["enemies"][i]["aggressiveness"],
                *roomIt,
                data["enemies"][i]["killedby"],
            });
        }

        objective = {
            data["objective"]["type"],
            data["objective"]["what"]
        };

        // Set initial player room
        std::string initialRoom = data["player"]["initialroom"];
        auto currentRoomIt = std::find_if(allRooms.begin(), allRooms.end(), [&initialRoom, this](const Room& room) {
            return initialRoom == room.id;
        });
        currentRoom = *(currentRoomIt);

        initializePlayerHP();
        showPlayerHP();
    }

    // Printing description of current room with object inside
    void showRoom() {
        std::cout << currentRoom.desc << std::endl;

        auto roomObjects = std::find_if(allObjects.begin(), allObjects.end(), [this](const Object& obj) {
            return obj.room.id == currentRoom.id;
        });

        if (roomObjects != allObjects.end()) {
            for (const auto& obj : allObjects) {
                if (obj.hidden == false && obj.room.id == currentRoom.id) {
                    std::cout << "You can see " << (isVowel(obj.id.front()) ? "an" : "a") << " " << obj.id << "." << std::endl;
                }
            }
        }

        // Printing enemies in the current room
        auto roomEnemies = std::find_if(allEnemies.begin(), allEnemies.end(), [this](const Enemy& enemy) {
            return enemy.initialRoom.id == currentRoom.id;
        });

        if (roomEnemies != allEnemies.end()) {
            for (const auto& enemy : allEnemies) {
                if (enemy.initialRoom.id == currentRoom.id) {
                    std::cout << "You can see enemy " << (isVowel(enemy.id.front()) ? "an" : "a") << " " << enemy.id << "." << std::endl;
                }
            }
        }

        // Printing food in the current room
        auto roomFood = std::find_if(allFood.begin(), allFood.end(), [this](const Food& food) {
            return food.room.id == currentRoom.id;
        });

        if (roomFood != allFood.end()) {
            for (const auto& food : allFood) {
                if (food.room.id == currentRoom.id) {
                    std::cout << "You can see " << (isVowel(food.id.front()) ? "an" : "a") << " " << food.id << "." << std::endl;
                }
            }
        }

        // Printing treasures in the current room
        auto roomTreasures = std::find_if(allTreasure.begin(), allTreasure.end(), [this](const Treasure& treasure) {
            return treasure.room.id == currentRoom.id;
        });

        if (roomTreasures != allTreasure.end()) {
            std::cout << "You see the following treasures:" << std::endl;
            for (const auto& treasure : allTreasure) {
                if (treasure.room.id == currentRoom.id) {
                    std::cout << "- " << treasure.id << std::endl;
                }
            }
        }

    }


    // All command that player can use in game
    void processCommand(const std::string& command) {
        if (command == "quit") {
            std::cout << "Goodbye! Thanks for playing." << std::endl;
            exit(0);
        } else if (command == "look") {
            showRoom();
        } else if (command == "inventory") {
            showInventory();
        } else if (command.find("look ") == 0) {
        std::string findObject = command.substr(5);
        showObject(findObject);
        } else if (command.find("take ") == 0) {
        std::string takeItemName = command.substr(5);
        takeItem(takeItemName);
        } else if (command.find("go ") == 0) {
        std::string direction = command.substr(3);
        goDirection(direction);
        } else if (command.find("kill ") == 0) {
        std::string enemyId = command.substr(5);
        killEnemy(enemyId);
        } else if (command.find("eat ") == 0) {
            std::string foodId = command.substr(4);
            eatFood(foodId);
        } else if (command.find("open ") == 0) {
            std::string treasureId = command.substr(5);
            openTreasure(treasureId);
        } else {
            std::cout << "Invalid command. Type 'look' to see your surroundings." << std::endl;
        }
    }

    // Checking objective to win the game
    bool checkObjective() {
        // Seting ojbective as killing enemy
        if (objective.type == "kill") {
            for (const std::string& enemyId : objective.what) {
                auto enemyIt = std::find_if(allEnemies.begin(), allEnemies.end(), [&enemyId, this](const Enemy& enemy) {
                    return enemy.id == enemyId;
                });

                if (enemyIt != allEnemies.end()) {
                    return false;
                }
            }
            return true;
        }
        // Setting objective as collecting required objects
        else if (objective.type == "collect") {
            for (const std::string& objectId : objective.what) {
                auto objectIt = std::find_if(inventory.begin(), inventory.end(), [&objectId, this](const Object& object) {
                    return object.id == objectId;
                });

                if (objectIt == inventory.end()) {
                    return false;
                }
            }
            return true;
        }
        // Setting objective as going into specific room
        else if (objective.type == "room") {
            if (currentRoom.id == objective.what[0]) {
                return true;
            }
            return false;
        }
        return false;
    }


private:
    struct Room {
        std::string id;
        std::string desc;
        std::map<std::string, std::string> exits;
    };

    struct Object {
        std::string id;
        std::string desc;
        Room room;
        bool hidden;
    };

    struct Enemy {
        std::string id;
        std::string desc;
        int aggressiveness;
        Room initialRoom;
        std::vector<std::string> killedBy;
    };

    struct Food {
        std::string id;
        std::string desc;
        Room room;
        int regeneration;
    };

    struct Treasure {
        std::string id;
        std::string desc;
        Room room;
        std::vector<std::string> openWith;
        std::vector<Object> contents;
    };

    struct Objective {
        std::string type;
        std::vector<std::string> what;
    };

    Room currentRoom;
    std::vector<Object> inventory;
    std::vector<Room> allRooms;
    std::vector<Object> allObjects;
    std::vector<Enemy> allEnemies;
    std::vector<Food> allFood;
    std::vector<Treasure> allTreasure;
    std::vector<Food> inventoryFood;
    Objective objective;

    int playerHP;
    int maxHP;

    



    // Function to check if a word starts with a vowel
    bool isVowel(char c) {
        return (c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u' ||
                c == 'A' || c == 'E' || c == 'I' || c == 'O' || c == 'U');
    }

    // Show player inventory with objects and food
    void showInventory() {
        if (inventory.empty() && inventoryFood.empty()) {
            std::cout << "Your inventory is empty." << std::endl;
        } else {
            std::cout << "You currently have:" << std::endl;

            for (auto& object : inventory) {
                std::cout << "A" << (isVowel(object.id.front()) ? "n" : "") << " " << object.id << std::endl;
            }

            for (auto& food : inventoryFood) {
                std::cout << "An " << food.id << std::endl;
            }
        }
    }


    void showObject(const std::string& findObject) {
        // Checking the current room for any object
        auto roomObjectIt = std::find_if(allObjects.begin(), allObjects.end(), [&findObject, this](const Object& obj) {
            return obj.id == findObject && obj.room.id == currentRoom.id;
        });

        // Checking the player's inventory for any object
        auto inventoryObjectIt = std::find_if(inventory.begin(), inventory.end(), [&findObject](const Object& obj) {
            return obj.id == findObject;
        });

        // Checking the current room any enemies
        auto enemyIt = std::find_if(allEnemies.begin(), allEnemies.end(), [&findObject, this](const Enemy& enemy) {
            return enemy.id == findObject && enemy.initialRoom.id == currentRoom.id;
        });

        // Checking the current room any food
        auto foodIt = std::find_if(allFood.begin(), allFood.end(), [&findObject, this](const Food& food) {
            return food.id == findObject && food.room.id == currentRoom.id;
        });

        // Checking the player's inventory for any food
        auto inventoryFoodIt = std::find_if(inventoryFood.begin(), inventoryFood.end(), [&findObject](const Food& food) {
            return food.id == findObject;
        });

        // Checking the current room any treasures
        auto treasureIt = std::find_if(allTreasure.begin(), allTreasure.end(), [&findObject, this](const Treasure& treasure) {
            return treasure.id == findObject && treasure.room.id == currentRoom.id;
        });


        if (roomObjectIt != allObjects.end() || enemyIt != allEnemies.end() || foodIt != allFood.end() || treasureIt != allTreasure.end()) {
            // Objects && enemies && foods or treasure is in the current room then writing its description
            std::cout << (roomObjectIt != allObjects.end() ? roomObjectIt->desc :
                        (enemyIt != allEnemies.end() ? enemyIt->desc :
                        (foodIt != allFood.end() ? foodIt->desc : treasureIt->desc))) << std::endl;
        } else if (inventoryObjectIt != inventory.end() || inventoryFoodIt != inventoryFood.end()) {
            // The object or food is in the player's inventory then writing its description
            std::cout << (inventoryObjectIt != inventory.end() ? inventoryObjectIt->desc : inventoryFoodIt->desc) << std::endl;
        } else {
            std::cout << "I don't see that here." << std::endl;
        }
    }

    // Taking the object or food to player's inventory fom specific room
    void takeItem(const std::string& item) {
        auto it = std::find_if(allObjects.begin(), allObjects.end(), [&item, this](const Object& obj) {
            return obj.id == item && obj.room.id == currentRoom.id;
        });

        auto foodIt = std::find_if(allFood.begin(), allFood.end(), [&item, this](const Food& food) {
            return food.id == item && food.room.id == currentRoom.id;
        });

        if (it != allObjects.end()) {
            inventory.push_back(*it);
            std::cout << "You take the " << item << "." << std::endl;
            allObjects.erase(it);
        } else if (foodIt != allFood.end()) {
            inventoryFood.push_back(*foodIt);
            std::cout << "You take the " << item << "." << std::endl;
            allFood.erase(foodIt);
        }else {
            std::cout << "I do not see that here." << std::endl;
        }
    }

    // Determining the exits in the room
    void goDirection(const std::string& direction) {
        auto it = currentRoom.exits.find(direction);

        if (it != currentRoom.exits.end()) {
            // Checking for any enemy in the current room
            auto enemyIt = std::find_if(allEnemies.begin(), allEnemies.end(), [&](const Enemy& enemy) {
                return enemy.initialRoom.id == currentRoom.id;
            });

            if (enemyIt != allEnemies.end()) {
                const Enemy& enemy = *enemyIt;

                std::srand(static_cast<unsigned>(std::time(nullptr)));

                std::random_device rd;
                std::mt19937 gen(rd());
                std::uniform_int_distribution<> dis(0, 99);

                // Determining if the enemy attacks based on aggressiveness
                if (dis(gen) < enemy.aggressiveness) {
                    std::cout << "The " << enemy.id << " attacked you and you were killed. Game over!" << std::endl;
                    exit(0);
                } else {
                    std::string nextRoomId = it->second;

                    auto nextRoomIt = std::find_if(allRooms.begin(), allRooms.end(), [nextRoomId](const Room& room) {
                        return room.id == nextRoomId;
                    });

                    if (nextRoomIt != allRooms.end()) {
                        currentRoom = *nextRoomIt;
                        showRoom();
                    } else {
                        std::cout << "Error: Room not found." << std::endl;
                    }
                }
            } else {
                // Moving to the next room without being attack by enemy
                std::string nextRoomId = it->second;

                auto nextRoomIt = std::find_if(allRooms.begin(), allRooms.end(), [nextRoomId](const Room& room) {
                    return room.id == nextRoomId;
                });

                if (nextRoomIt != allRooms.end()) {
                    currentRoom = *nextRoomIt;
                    showRoom();
                } else {
                    std::cout << "Error: Room not found." << std::endl;
                }
            }
        } else {
            std::cout << "You can't go that way." << std::endl;
        }
    }


    // creating 'kill xxx' function to kill the enemy
    void killEnemy(const std::string& enemyId) {
        // Checking if the player has the required items to kill the enemy
        auto enemyIt = std::find_if(allEnemies.begin(), allEnemies.end(), [&enemyId, this](const Enemy& enemy) {
            return enemy.id == enemyId && enemy.initialRoom.id == currentRoom.id;
        });

        if (enemyIt != allEnemies.end()) {
            const Enemy& enemy = *enemyIt;

            if (enemy.killedBy.empty() || std::all_of(enemy.killedBy.begin(), enemy.killedBy.end(), [&](const std::string& requiredItem) {
                return std::any_of(inventory.begin(), inventory.end(), [&](const Object& obj) {
                    return obj.id == requiredItem;
                });
            })) {

                std::cout << "You killed the " << enemyId << "." << std::endl;
                allEnemies.erase(enemyIt);
            } else {
                enemyAttack(enemy);
            }
        } else {
            std::cout << "I don't see that enemy here." << std::endl;
        }
    }

    // Function to initialize the player's health points
    void initializePlayerHP() {
        playerHP = 100;
        maxHP = 100;
    }

    void showPlayerHP() {
        std::cout << "You currently have [HP: " << playerHP << "/100]" << std::endl;
    }

    // Function that handle enemy attacks and reduce player's health points with random no
    void enemyAttack(const Enemy& enemy) {
        std::srand(static_cast<unsigned>(std::time(nullptr)));

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(10, 50);

        int damage = dis(gen);
        playerHP -= damage;

        std::cout << "The " << enemy.id << " attacked you and caused " << damage << " damage." << std::endl;

        if (playerHP <= 0) {
            std::cout << "Your health points reached zero. Game over!" << std::endl;
            exit(0);
        } else {
            showPlayerHP();
        }
    }

    // Function to handle eating foods and regenerate player's health points
    void eatFood(const std::string& foodId) {
        auto foodIt = std::find_if(inventoryFood.begin(), inventoryFood.end(), [&foodId, this](const Food& food) {
            return food.id == foodId;
        });

        if (foodIt != inventoryFood.end()) {
            const Food& food = *foodIt;

            std::cout << "You eat the " << food.id << "." << std::endl;

            playerHP += food.regeneration;

            playerHP = std::min(playerHP, maxHP);

            showPlayerHP();

            inventoryFood.erase(foodIt);
        } else {
            std::cout << "You don't have that food in your inventory." << std::endl;
        }
    }

    // Function to open the treasure and giving the object from treasure
    void openTreasure(const std::string& treasureId) {
        auto treasureIt = std::find_if(allTreasure.begin(), allTreasure.end(), [&treasureId, this](const Treasure& treasure) {
            return treasure.id == treasureId && treasure.room.id == currentRoom.id;
        });

        if (treasureIt != allTreasure.end()) {
            const Treasure& treasure = *treasureIt;

            if (treasure.openWith.empty() || std::all_of(treasure.openWith.begin(), treasure.openWith.end(), [&](const std::string& requiredItem) {
                return std::any_of(inventory.begin(), inventory.end(), [&](const Object& obj) {
                    return obj.id == requiredItem;
                });
            })) {
                std::cout << "You open the " << treasure.id << " and find:" << std::endl;

                // Adding the content of treasure to his inventory
                for (const auto& content : treasure.contents) {
                    inventory.push_back(content);
                    std::cout << content.id << std::endl;
                }

                allTreasure.erase(treasureIt);

            } else {
                std::cout << "The " << treasure.id << " is locked. You need the right items to open it." << std::endl;
            }
        } else {
            std::cout << "I don't see that treasure here." << std::endl;
        }
    }







};

int main(int argc, char *argv[]) {

    std::ifstream f(argv[1]);
    json data = json::parse(f);

    AdvGame game(data);
    std::string command;
    
    game.showRoom();

    while (true) {
        std::cout << "> ";
        std::getline(std::cin, command);
        game.processCommand(command);

        if (game.checkObjective()){
            std::cout << "Congratulations you win the game!" << std::endl;
            break;
        }
    }


    return 0;
}
