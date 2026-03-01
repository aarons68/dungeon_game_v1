#ifndef INVENTORY_H
#define INVENTORY_H

#include "raylib.h"
#include <vector>
#include <string>

// Struct to handle the "selected" vector (x, y, id)
struct Selected {
    int x;
    int y;
    int id;
    bool active = false;
};

struct Int2 {
    int x; int y;
};

//***************************START INVENTORYITEM CLASS*************************
class InventoryItem {
public:
    std::string type;
    int invItemId;
    int width;
    int height;
    Int2 position;
    bool isRotated;

    //START CONSTRUCTOR
    InventoryItem(std::string type = "none", int id = 0, int width = 1, int height = 1, Int2 position = {0,0});
    // END CONSTRUCTOR
};

//***************************START INVENTORY CLASS***************************
class Inventory {
public:
    int invSize;
    std::vector<std::vector<InventoryItem>> grid;
    float invTileSize;

    //START CONSTRUCTOR
    Inventory();
    // END CONSTRUCTOR

    // START drawInventory
    void drawInventory();
};

// Functions from your JS helpers
void inventoryMouseHandler();
void placeObject(Inventory& srcInventory, Inventory& targetInv, Selected selected, Int2 target);

#endif