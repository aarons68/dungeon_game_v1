#ifndef INVENTORY_H
#define INVENTORY_H

#include "raylib.h"
#include "constants.h"
#include <vector>
#include <string>

class Player;
class Inventory;

// Struct to handle the "selected" vector (x, y, id)
struct Selected {
    int x;
    int y;
    int id;
    bool active = false;
    Inventory* sourceInv = nullptr;
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

    //START CONSTRUCTOR
    Inventory(int size=INV_START_SIZE);
    // END CONSTRUCTOR

    // START drawInventory
    void drawInventory(Selected& selected, float xOffset = 0.0f, float scale = 1.0f);
};

// helpers
void inventoryMouseHandler(Inventory& inv, Selected& selected, float xOffset = 0.0f, float scale = 1.0f);
void placeObject(Inventory& target, Selected& selected, Int2 targetPos);

float calcTileSize(float scale);

#endif