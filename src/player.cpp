#include "../include/player.h" // This includes raylib.h inside of it
#include "../include/constants.h"
#define RAYMATH_STATIC_INLINE
#include "raymath.h"           // Include this AFTER your player header

extern int worldMap[MAP_HEIGHT][MAP_WIDTH];

// START CONSTRUCTOR
Player::Player(float x, float y) {
    // health and position 
    this->health = PLAYER_MAX_HEALTH;
    this->pos = {x, y};

    // inventory management
    this->currInvSelected.active = false;

    this->nearChest = false;
    this->nearExit = false;
}
// END CONSTRUCTOR

// START drawPlayer
void Player::drawPlayer(std::string gameState) {
    DrawRectangle(this->pos.x - PLAYER_SIZE/2, this->pos.y - PLAYER_SIZE/2, PLAYER_SIZE, PLAYER_SIZE, RAYWHITE);
}

//START updatePlayer
void Player::updatePlayer(std::string& gameState, bool& insideInventory, int worldMap[MAP_HEIGHT][MAP_WIDTH], std::vector<Chest>& chests, Chest*& openChest) {
    // call movement updating
    this->playerMovement(worldMap);
    this->checkInteractions(gameState, insideInventory, worldMap, chests, openChest);
}

// START PLAYER MOVEMENT
void Player::playerMovement(int worldMap[MAP_HEIGHT][MAP_WIDTH]) {
    float dx = 0; float dy = 0;
    if (IsKeyDown(KEY_W)) dy -= PLAYER_SPEED;
    if (IsKeyDown(KEY_S)) dy += PLAYER_SPEED;
    if (IsKeyDown(KEY_A)) dx -= PLAYER_SPEED;
    if (IsKeyDown(KEY_D)) dx += PLAYER_SPEED;

    auto isPhysicalWall = [&](float x, float y, float moveX, float moveY) {
        //find which tile 
        int tx = static_cast<int>(x / TILE_SIZE);
        int ty = static_cast<int>(y / TILE_SIZE);

        if (tx < 0 || tx >= MAP_WIDTH || ty < 0 || ty >= MAP_HEIGHT) return true;
        if (worldMap[ty][tx] == 0) return false; // Floor is always safe

        //calculate position in tile
        float localX = fmodf(x, (float)TILE_SIZE);
        if (localX < 0) localX += TILE_SIZE;

        // for collision with walls on right
        if (moveX > 0 && localX < PLAYER_COLL_PADDING) return false; 
        //for collision with walls on left
        if (moveX < 0 && localX > (TILE_SIZE - PLAYER_COLL_PADDING)) return false;

        return true;
    };

    float halfSize = (PLAYER_SIZE / 2.0f) - 1.0f;

    // try x
    float nextX = this->pos.x + dx;
    float checkX = nextX + (dx > 0 ? halfSize : -halfSize);
    
    bool xCollision = isPhysicalWall(checkX, this->pos.y - halfSize, dx, 0) || 
                      isPhysicalWall(checkX, this->pos.y + halfSize, dx, 0);

    if (!xCollision) this->pos.x = nextX;

    // try y
    float nextY = this->pos.y + dy;
    float checkY = nextY + (dy > 0 ? halfSize : -halfSize);

    bool yCollision = isPhysicalWall(this->pos.x - halfSize, checkY, 0, dy) || 
                      isPhysicalWall(this->pos.x + halfSize, checkY, 0, dy);

    if (!yCollision) this->pos.y = nextY;

    // screen clamping 
    this->pos.x = Clamp(this->pos.x, halfSize, (float)TILE_SIZE * MAP_WIDTH - halfSize);
    this->pos.y = Clamp(this->pos.y, halfSize, (float)TILE_SIZE * MAP_HEIGHT - halfSize);
    

    //updating player angle
    if (dx != 0 || dy != 0) {
        this->playerAngle = atan2f(dy, dx) * RAD2DEG;
    }
    
}

// START openInventory
void Player::openInventory(Chest* openChest) {
    ClearBackground(GRAY);
    if (openChest == nullptr) {
        // solo — full size, centered
        this->playerInventory.drawInventory(this->currInvSelected, 0.0f, 1.0f);
    } else {
        // two inventories side by side — scale down to half
        this->playerInventory.drawInventory(this->currInvSelected, 0.0f, 0.5f);
        openChest->inventory.drawInventory(this->currInvSelected, (float)DISPLAY_SIZE / 2.0f, 0.5f);
    }
}

void Player::checkInteractions(std::string& gameState, bool& insideInventory, int worldMap[MAP_HEIGHT][MAP_WIDTH], std::vector<Chest>& chests, Chest*& openChest) {
    // Convert player world position to grid coordinates
    int tx = (int)(this->pos.x / TILE_SIZE);
    int ty = (int)(this->pos.y / TILE_SIZE);

    this->nearChest = false;
    this->nearExit = false;

    // check in 3x3 surrounding tiles for chest or exit
    for (int y = -1; y <= 1; y++) {
        for (int x = -1; x <= 1; x++) {
            int cx = tx + x;
            int cy = ty + y;

            if (cx >= 0 && cx < MAP_WIDTH && cy >= 0 && cy < MAP_HEIGHT) {
                if (worldMap[cy][cx] == 2) {
                    this->nearChest = true;
                    
                    //if f near chest
                    if (IsKeyPressed(KEY_F)) {
                        for (auto& chest : chests) {
                        if (chest.mapX == cx && chest.mapY == cy) {
                            openChest = &chest;
                            break;
                        }
                    }
                        gameState = "inventory";
                        insideInventory = true;
                    }
                }
                if (worldMap[cy][cx] == 3) {
                    this->nearExit = true;
                    //press f near exit
                    if (IsKeyPressed(KEY_F)) {
                        //do sum 
                        return;
                    }
                }
            }
        }
    }
}