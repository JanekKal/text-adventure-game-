In this readme file you will find the information of the engine of the game and how the game is working.

Firstly, you will need to compile the game with makefile that is provided with this game.
To compile simply write "make" in your terminal.

To run the game write "./main map.json" to run a specific map.

Please, feel free to use the "pirate.json" map to explore the pirate world.

In the game there are features and functions as:

###Simple features:

Descrtiption of location:
    After entering a location or writing a "look" command, the description of location will be prompted to the user.
    
Description of items:
    Every specific object, location and enemy have their own description.
    User by using the command "look xxx" can see the description of the specified requirement.
    
Taking items:
    Within different location there are many objects that could be picked by the user.
    Command "take xxx" allow user to collect the items into the inventory.
    
Getting inventory:
    User can see their inventory using "inventory" command to see what items are collected.
    
Navigation through map:
    User can change the locations using the "go xxx" command, that will take the user into another location.
    
###Advanced features:

Health points & combat as function to hanadle enemy attack and reduce health points:
    Player will gain [HP: 100/100] health points at the beginning of the game. 
    During the fight with enemy, if the player do not have specific objects to kill the enemy, the damage to player will be given.
    However, the player can restore some of health points eating the food picked from different rooms.
    Additionaly, player can still be imediately killed when escaping from the room where is the living enemy.
Examples:
    On the start of the game player have [HP: 100/100] health points.
    Player goes to room2 where the enemy is located. Player tries to kill the enemy, but does not has specific objects to kill that enemy.
    The damage is randomly generated between (10 to 50) damage points.
    The information will be printed that: 
    "The enemy attacked you and caused x damage.
    You currently have [HP: 100-x/100]"
    
Escaping from room:
    If the player could not killed the enemy, he need to find the specific objects to kill. However, if the player tries to leave without killing enemy first, the enemy will attack - which cause dying imadietly - and whether it succeeds depends on its aggressiveness.
Health points allow the player to play longer if he is lucky enough to leave rooms where the enemy is still alive.

Food feature and "eat xxx" function:
    In game player can find the food in some locations.
    If the player want to see the treausre description can write the command "look xxx" to see its description.
    The food can be picked up to the players inventory. 
    Very similar to the objects feature, but it has additional use.
    Additionaly, if the player will lose their health points during the combat with enemy. 
    The food can be eaten which will be increasing the player's health points.
    After a combat with enemy the player has for example: [HP: 10/100]. If the food was picked to the inventory, only then can be eaten.
    After eating the food, the health points will increase by the regeneration points.
    Food has regeneration points, which are defined in the same way as "aggressiveness" from enemy.
    The regeneration points, indicate how much they can heal the player. 
Example:
    Rum has regeneration 80. When the player have [HP: 10/100], he can use the command as:
    "eat rum" (if only he has rum in the inventory)
    The health points will increase by the number of regeneration points. In this case, the player will receive 80 health points, and the information will be printed:
    "You eat the rum.
    You currently have [HP: 90/100]."
    However, if the player has [HP:50/100] and he eat the food that has 80 regeneration points, the health points it will only increase to his maximum health points which is 100.

Treasure and function "open xxx":
    On the map, player can find the treasure. It can only be open with specific object from the player's inventory. The "treasure" cannot be picked up to the player's inventory. If the player want to see the treausre description can write the command "look xxx" to see its description. After opening the the treausre, the player will be given an object to his inventory.
Example:
    The player found a treasure as chest:
    "You see the following treasures:
    - chest"
    after trying to open that without specific object the information will be printed:
    ">open chest
    The chest is locked. You need the right items to open it."
    However, if the player find the specific object to open that treasure and try to open, it will print the information as:
    "> open chest
    You open the chest and find:
    Gold of the Pirate"
    Which will automatically add the object as in this case "Gold of the Pirate" to the players inventory. 


