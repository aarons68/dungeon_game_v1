#ifndef CHEST_H
#define CHEST_H

#include "inventory.h"

struct Chest {
    int mapX;
    int mapY;
    Inventory inventory;
};

#endif