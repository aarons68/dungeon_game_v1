#include "../include/inventory.h"
#include "../include/player.h"
#include "../include/constants.h"

// Accessing globals from main.cpp
extern Player* playerObject;
extern int invIdCounter;
const float padding_factor = 6.0f;

InventoryItem::InventoryItem(std::string type, int id, int width, int height, Int2 position) {
    this->type = type;
    this->invItemId = id;
    this->width = width;
    this->height = height;
    this->position = position;
    this->isRotated = false;
}

Inventory::Inventory() {
    this->invSize = INV_START_SIZE;
    this->grid.resize(this->invSize, std::vector<InventoryItem>(this->invSize, InventoryItem("none")));

    // Dynamic tile size calculation based on DISPLAY_SIZE
    this->invTileSize = (2.0f * (static_cast<float>(DISPLAY_SIZE) / 3.0f)) / static_cast<float>(this->invSize);

    this->grid[4][5] = InventoryItem("sword", 44, 1, 2, {0,0});
    this->grid[5][5] = InventoryItem("sword", 44, 1, 2, {0,1});

    this->grid[7][8] = InventoryItem("sword", 22, 1, 3, {0,0});
    this->grid[8][8] = InventoryItem("sword", 22, 1, 3, {0,1});        
    this->grid[9][8] = InventoryItem("sword", 22, 1, 3, {0,2});

    this->grid[1][8] = InventoryItem("sword", 34, 2, 3, {0,0});
    this->grid[2][8] = InventoryItem("sword", 34, 2, 3, {0,1});        
    this->grid[3][8] = InventoryItem("sword", 34, 2, 3, {0,2});
    this->grid[1][9] = InventoryItem("sword", 34, 2, 3, {1,0});
    this->grid[2][9] = InventoryItem("sword", 34, 2, 3, {1,1});        
    this->grid[3][9] = InventoryItem("sword", 34, 2, 3, {1,2});
}

// START drawInventory
void Inventory::drawInventory() {
    // xpos and ypos to be used layer
    float xPos;
    float yPos;
    
    ClearBackground(GRAY);
    // draw each square
    for(int i = 0; i < this->invSize; i++){
        for(int j = 0; j < this->invSize; j++){
            // Use DISPLAY_SIZE to keep inventory centered at 2/3 of screen
            xPos = static_cast<float>(DISPLAY_SIZE)/padding_factor + j * this->invTileSize;
            yPos = static_cast<float>(DISPLAY_SIZE)/padding_factor + i * this->invTileSize;
            
            if(!playerObject->currInvSelected.active || this->grid[i][j].invItemId != playerObject->currInvSelected.id){
                // switch for items in grid
                if(this->grid[i][j].type == "none") {
                    DrawRectangleLines(xPos, yPos, this->invTileSize, this->invTileSize, BLACK);
                } else if(this->grid[i][j].type == "sword") {
                    DrawRectangle(xPos, yPos, this->invTileSize, this->invTileSize, YELLOW);
                    DrawRectangleLines(xPos, yPos, this->invTileSize, this->invTileSize, BLACK);
                }
            } else {
                DrawRectangleLines(xPos, yPos, this->invTileSize, this->invTileSize, BLACK);
            }
        }
    }

    if(playerObject->currInvSelected.active){
        // Grab the actual object data from the grid
        int selectedX = playerObject->currInvSelected.x;
        int selectedY = playerObject->currInvSelected.y;
        InventoryItem& selectedObject = this->grid[selectedY][selectedX];
        
        int width = selectedObject.width;
        int height = selectedObject.height;

        if(selectedObject.isRotated){
            width = selectedObject.height;
            height = selectedObject.width;
        }

        // Loop through the width and height of the object to draw all its tiles
        for(int i = 0; i < height; i++){
            for(int j = 0; j < width; j++){
                // Calculate relative drawing position (centered on mouse)
                float drawPosX = (static_cast<float>(GetMouseX()) - this->invTileSize/2.0f) + (static_cast<float>(j) - static_cast<float>(selectedObject.position.x)) * this->invTileSize;
                float drawPosY = (static_cast<float>(GetMouseY()) - this->invTileSize/2.0f) + (static_cast<float>(i) - static_cast<float>(selectedObject.position.y)) * this->invTileSize;
                
                DrawRectangle(drawPosX, drawPosY, this->invTileSize, this->invTileSize, YELLOW); 
                DrawRectangleLines(drawPosX, drawPosY, this->invTileSize, this->invTileSize, BLACK);
            }
        }
    }
}
// END drawInventory

// START inventoryMouseHandler
void inventoryMouseHandler(){
  //if mouse in inventory bounds
  float mX = static_cast<float>(GetMouseX());
  float mY = static_cast<float>(GetMouseY());
  float boundsStart = static_cast<float>(DISPLAY_SIZE) / padding_factor;
  float boundsEnd = static_cast<float>(DISPLAY_SIZE) - boundsStart;

    if(mX > boundsStart && mX < boundsEnd && mY > boundsStart && mY < boundsEnd){
        Int2 mouseTile;

        mouseTile.x = static_cast<int>((mX - boundsStart) / playerObject->playerInventory.invTileSize);
        mouseTile.y = static_cast<int>((mY - boundsStart) / playerObject->playerInventory.invTileSize);
        
        if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
            //first if player has no object selected
            if(!playerObject->currInvSelected.active){
                //if clicking empty tile, select this as new object
                if(playerObject->playerInventory.grid[mouseTile.y][mouseTile.x].type != "none"){
                    playerObject->currInvSelected = {mouseTile.x, mouseTile.y, playerObject->playerInventory.grid[mouseTile.y][mouseTile.x].invItemId, true};
                }   
            }
            //if player has object selected
            else{
                if(playerObject->playerInventory.grid[mouseTile.y][mouseTile.x].type == "none" || playerObject->playerInventory.grid[mouseTile.y][mouseTile.x].invItemId == playerObject->playerInventory.grid[playerObject->currInvSelected.y][playerObject->currInvSelected.x].invItemId){
                    placeObject(playerObject->playerInventory, playerObject->playerInventory, playerObject->currInvSelected, mouseTile);
                }
            }
        }
    }
}
// END inventoryMouseHandler

// START placeObject
void placeObject(Inventory& srcInventory, Inventory& targetInv, Selected selected, Int2 target){
    InventoryItem& selectedObject = srcInventory.grid[selected.y][selected.x];

    //if 1x1 item
    if(selectedObject.height == 1 && selectedObject.width == 1){
        targetInv.grid[target.y][target.x] = srcInventory.grid[selected.y][selected.x];
        srcInventory.grid[selected.y][selected.x] = InventoryItem("none");
        playerObject->currInvSelected.active = false;
    }
    //if larger than 1x1 item
    else{
        int height;
        int width;
        if(!selectedObject.isRotated){
            height = selectedObject.height;
            width = selectedObject.width;
        }
        else{
            width = selectedObject.height;
            height = selectedObject.width;
        }
        
        Int2 targetStartingPos = {target.x - selectedObject.position.x, target.y - selectedObject.position.y};

        //check if in bounds
        if(targetStartingPos.x < 0 || targetStartingPos.x + width > targetInv.invSize || targetStartingPos.y < 0 || targetStartingPos.y + height > targetInv.invSize){
            playerObject->currInvSelected.active = false;
            return;
        }
        
        //check if empty or same item id first
        for(int i = 0; i < height; i++){
            for(int j = 0; j < width; j++)
            {
                if(targetInv.grid[i + targetStartingPos.y][targetStartingPos.x + j].type != "none" && targetInv.grid[i + targetStartingPos.y][targetStartingPos.x + j].invItemId != selectedObject.invItemId){
                    return;
                }
            }
        }
                
        //put them all on a vector 
        std::vector<InventoryItem> clipboard;
        int heldId = selectedObject.invItemId;

        for(int i = 0; i < srcInventory.invSize; i++){
            for(int j = 0; j < srcInventory.invSize; j++) {
                if(srcInventory.grid[i][j].invItemId == heldId){
                    clipboard.push_back(srcInventory.grid[i][j]);
                    // erase from the real grid
                    srcInventory.grid[i][j] = InventoryItem("none");
                }
            }
        }

        //put them back into the correct position
        for(const auto& piece : clipboard) {
            int finalX = targetStartingPos.x + piece.position.x;
            int finalY = targetStartingPos.y + piece.position.y;
            targetInv.grid[finalY][finalX] = piece;
        }
        
        playerObject->currInvSelected.active = false;
    }
}
// END placeObject