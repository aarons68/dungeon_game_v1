#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"
#include "inventory.h"

//***************************START PLAYER CLASS***************************
class Player {
public:
    int health;
    Vector2 pos;
    Selected currInvSelected;
    Inventory playerInventory;
    float playerAngle;


    // START CONSTRUCTOR
    Player(float x, float y);
    // END CONSTRUCTOR

    // START drawPlayer
    void drawPlayer();
    
    //START updatePlayer
    void updatePlayer();

    // START PLAYER MOVEMENT
    void playerMovement();

    // START openInventory
    void openInventory();

    void checkInteractions();
};

#endif