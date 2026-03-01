#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"
#include "inventory.h"
#include "constants.h"
#include "chest.h"
#include <vector>

//***************************START PLAYER CLASS***************************
class Player {
public:
    int health;
    Vector2 pos;
    Selected currInvSelected;
    Inventory playerInventory;
    float playerAngle;

    bool nearChest;
    bool nearExit;


    // START CONSTRUCTOR
    Player(float x, float y);
    // END CONSTRUCTOR

    // START drawPlayer
    void drawPlayer(std::string gameState);
    
    //START updatePlayer
    void updatePlayer(std::string& gameState, bool& insideInventory, int worldMap[MAP_HEIGHT][MAP_WIDTH], std::vector<Chest>& chests, Chest*& openChest);

    // START PLAYER MOVEMENT
    void playerMovement(int worldMap[MAP_HEIGHT][MAP_WIDTH]) ;

    // START openInventory
    void openInventory(Chest* openChest);

    void checkInteractions(std::string& gameState, bool& insideInventory, int worldMap[MAP_HEIGHT][MAP_WIDTH], std::vector<Chest>& chests, Chest*& openChest);
};

#endif