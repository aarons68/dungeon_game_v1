#include "../include/inventory.h"
#include "../include/player.h"
#include "../include/constants.h"

const float padding_factor = 6.0f;
extern int invIdCounter;

InventoryItem::InventoryItem(std::string type, int id, int width, int height, Int2 position) {
    this->type = type;
    this->invItemId = id;
    this->width = width;
    this->height = height;
    this->position = position;
    this->isRotated = false;
}

Inventory::Inventory(int size) {
    this->invSize = size;
    this->grid.resize(this->invSize, std::vector<InventoryItem>(this->invSize, InventoryItem("none")));


    this->grid[3][1] = InventoryItem("sword", invIdCounter, 1, 2, {0,0});
    this->grid[4][1] = InventoryItem("sword", invIdCounter, 1, 2, {0,1});
    invIdCounter ++;

    this->grid[0][2] = InventoryItem("sword", invIdCounter, 1, 3, {0,0});
    this->grid[0][2] = InventoryItem("sword", invIdCounter, 1, 3, {0,1});        
    this->grid[0][2] = InventoryItem("sword", invIdCounter, 1, 3, {0,2});
    invIdCounter ++;

    this->grid[1][3] = InventoryItem("sword", invIdCounter, 2, 3, {0,0});
    this->grid[2][3] = InventoryItem("sword", invIdCounter, 2, 3, {0,1});        
    this->grid[3][3] = InventoryItem("sword", invIdCounter, 2, 3, {0,2});
    this->grid[1][4] = InventoryItem("sword", invIdCounter, 2, 3, {1,0});
    this->grid[2][4] = InventoryItem("sword", invIdCounter, 2, 3, {1,1});        
    this->grid[3][4] = InventoryItem("sword", invIdCounter, 2, 3, {1,2});
    invIdCounter ++;
}

// START drawInventory
void Inventory::drawInventory(Selected& selected, float xOffset, float scale) {
    float tileSize = calcTileSize(scale);
    float totalWidth = tileSize * this->invSize;
    float totalHeight = tileSize * this->invSize;

    float halfScreen = (float)DISPLAY_SIZE / 2.0f;
    float startY = ((float)DISPLAY_SIZE - totalHeight) / 2.0f;
    float startX;

    if(scale < 1.0f) {
        startX = xOffset + (halfScreen - totalWidth) / 2.0f;
    } else {
        startX = ((float)DISPLAY_SIZE - totalWidth) / 2.0f;
    }

    for(int i = 0; i < this->invSize; i++){
        for(int j = 0; j < this->invSize; j++){
            float xPos = startX + j * tileSize;
            float yPos = startY + i * tileSize;
            
            if(!selected.active || this->grid[i][j].invItemId != selected.id){
                if(this->grid[i][j].type == "none") {
                    DrawRectangleLines(xPos, yPos, tileSize, tileSize, BLACK);
                } else if(this->grid[i][j].type == "sword") {
                    DrawRectangle(xPos, yPos, tileSize, tileSize, YELLOW);
                    DrawRectangleLines(xPos, yPos, tileSize, tileSize, BLACK);
                }
            } else {
                DrawRectangleLines(xPos, yPos, tileSize, tileSize, BLACK);
            }
        }
    }

    if(selected.active && selected.sourceInv == this){
        int selectedX = selected.x;
        int selectedY = selected.y;
        InventoryItem& selectedObject = this->grid[selectedY][selectedX];
        
        int width = selectedObject.isRotated ? selectedObject.height : selectedObject.width;
        int height = selectedObject.isRotated ? selectedObject.width : selectedObject.height;

        for(int i = 0; i < height; i++){
            for(int j = 0; j < width; j++){
                float drawPosX = (static_cast<float>(GetMouseX()) - tileSize/2.0f) + (static_cast<float>(j) - static_cast<float>(selectedObject.position.x)) * tileSize;
                float drawPosY = (static_cast<float>(GetMouseY()) - tileSize/2.0f) + (static_cast<float>(i) - static_cast<float>(selectedObject.position.y)) * tileSize;
                
                DrawRectangle(drawPosX, drawPosY, tileSize, tileSize, YELLOW); 
                DrawRectangleLines(drawPosX, drawPosY, tileSize, tileSize, BLACK);
            }
        }
    }
}
// END drawInventory

//helper for multiple sized inventories
float calcTileSize(float scale) {
    float regionWidth = (float)DISPLAY_SIZE / (scale < 1.0f ? 2.0f : 1.0f);
    float regionHeight = (float)DISPLAY_SIZE;
    float padding = 100.0f;
    
    float tileFromWidth = (regionWidth - padding) / INV_MAX_SIZE;
    float tileFromHeight = (regionHeight - padding) / INV_MAX_SIZE;
    
    return std::min(tileFromWidth, tileFromHeight);
}

// START inventoryMouseHandler
void inventoryMouseHandler(Inventory& inv, Selected& selected, float xOffset, float scale) {
    float tileSize = calcTileSize(scale);
    float totalWidth = tileSize * inv.invSize;
    float totalHeight = tileSize * inv.invSize;
    
    float halfScreen = (float)DISPLAY_SIZE / 2.0f;
    float startY = ((float)DISPLAY_SIZE - totalHeight) / 2.0f;
    float startX;
    
    if(scale < 1.0f) {
        startX = xOffset + (halfScreen - totalWidth) / 2.0f;
    } else {
        startX = ((float)DISPLAY_SIZE - totalWidth) / 2.0f;
    }

    float mX = static_cast<float>(GetMouseX());
    float mY = static_cast<float>(GetMouseY());

    if(mX > startX && mX < startX + totalWidth && mY > startY && mY < startY + totalHeight){
        Int2 mouseTile;
        mouseTile.x = static_cast<int>((mX - startX) / tileSize);
        mouseTile.y = static_cast<int>((mY - startY) / tileSize);

        if(mouseTile.x < 0 || mouseTile.x >= inv.invSize || mouseTile.y < 0 || mouseTile.y >= inv.invSize) return;
        
        if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
            if(!selected.active){
                if(inv.grid[mouseTile.y][mouseTile.x].type != "none"){
                    selected = {mouseTile.x, mouseTile.y, inv.grid[mouseTile.y][mouseTile.x].invItemId, true, &inv};
                }   
            } else {
                if(inv.grid[mouseTile.y][mouseTile.x].type == "none" || inv.grid[mouseTile.y][mouseTile.x].invItemId == inv.grid[selected.y][selected.x].invItemId){
                    placeObject(inv, selected, mouseTile);
                }
            }
        }
    }
}
// END inventoryMouseHandler

// START placeObject
void placeObject(Inventory& target, Selected& selected, Int2 targetPos){
    if(selected.sourceInv == nullptr) return;
    Inventory& src = *selected.sourceInv;
    InventoryItem& selectedObject = src.grid[selected.y][selected.x];


    //if 1x1 item
    if(selectedObject.height == 1 && selectedObject.width == 1){
        target.grid[targetPos.y][targetPos.x] = src.grid[selected.y][selected.x];
        src.grid[selected.y][selected.x] = InventoryItem("none");
        selected.active = false;
        selected.sourceInv = nullptr;
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
        
        Int2 targetStartingPos = {targetPos.x - selectedObject.position.x, targetPos.y - selectedObject.position.y};

        //check if in bounds
        if(targetStartingPos.x < 0 || targetStartingPos.x + width > target.invSize || targetStartingPos.y < 0 || targetStartingPos.y + height > target.invSize){
            selected.active = false;
            return;
        }
        
        //check if empty or same item id first
        for(int i = 0; i < height; i++){
            for(int j = 0; j < width; j++)
            {
                if(target.grid[i + targetStartingPos.y][targetStartingPos.x + j].type != "none" && target.grid[i + targetStartingPos.y][targetStartingPos.x + j].invItemId != selectedObject.invItemId){
                    return;
                }
            }
        }
                
        //put them all on a vector 
        std::vector<InventoryItem> clipboard;
        int heldId = selectedObject.invItemId;

        for(int i = 0; i < src.invSize; i++){
            for(int j = 0; j < src.invSize; j++) {
                if(src.grid[i][j].invItemId == heldId){
                    clipboard.push_back(src.grid[i][j]);
                    // erase from the real grid
                    src.grid[i][j] = InventoryItem("none");
                }
            }
        }

        //put them back into the correct position
        for(const auto& piece : clipboard) {
            int finalX = targetStartingPos.x + piece.position.x;
            int finalY = targetStartingPos.y + piece.position.y;
            target.grid[finalY][finalX] = piece;
        }
        
        selected.active = false;
        selected.sourceInv = nullptr;
    }
}
// END placeObject