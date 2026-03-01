#include "../include/player.h" // This includes raylib.h inside of it
#include "../include/constants.h"
#define RAYMATH_STATIC_INLINE
#include "raymath.h"           // Include this AFTER your player header

extern Player* playerObject;
extern std::string gameState;
extern bool insideInventory; 

// START CONSTRUCTOR
Player::Player(float x, float y) {
    // health and position 
    this->health = PLAYER_MAX_HEALTH;
    this->pos = {x, y};

    // inventory management
    this->currInvSelected.active = false;
}
// END CONSTRUCTOR

// START drawPlayer
void Player::drawPlayer() {
    DrawRectangle(this->pos.x - PLAYER_SIZE/2, this->pos.y - PLAYER_SIZE/2, PLAYER_SIZE, PLAYER_SIZE, RAYWHITE);
}

//START updatePlayer
void Player::updatePlayer() {
    // call movement updating
    this->playerMovement();
    this->checkInteractions();
}

// START PLAYER MOVEMENT
void Player::playerMovement() {
    float dx = 0; float dy = 0;
    if (IsKeyDown(KEY_W)) dy -= PLAYER_SPEED;
    if (IsKeyDown(KEY_S)) dy += PLAYER_SPEED;
    if (IsKeyDown(KEY_A)) dx -= PLAYER_SPEED;
    if (IsKeyDown(KEY_D)) dx += PLAYER_SPEED;

    // The "Padding" represents the empty space in a 32px tile (10px on each side)
    float padding = 10.0f; 

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
        if (moveX > 0 && localX < padding) return false; 
        //for collision with walls on left
        if (moveX < 0 && localX > (TILE_SIZE - padding)) return false;

        return true; // If we've pushed past the padding, it's a hit
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
void Player::openInventory() {
    this->playerInventory.drawInventory();
}

void Player::checkInteractions() {
    // Convert player world position to grid coordinates
    int tx = (int)(this->pos.x / TILE_SIZE);
    int ty = (int)(this->pos.y / TILE_SIZE);

    bool nearChest = false;
    bool nearExit = false;

    // Check 3x3 area around player for a '2' (Chest)
    for (int y = -1; y <= 1; y++) {
        for (int x = -1; x <= 1; x++) {
            int cx = tx + x;
            int cy = ty + y;

            if (cx >= 0 && cx < MAP_WIDTH && cy >= 0 && cy < MAP_HEIGHT) {
                if (worldMap[cy][cx] == 2) {
                    nearChest = true;
                    
                    // If player presses F while near a chest
                    if (IsKeyPressed(KEY_F)) {
                        gameState = "inventory";
                        insideInventory = true;
                        // Optional: worldMap[cy][cx] = 0; // Remove chest after opening
                    }
                }
                if (worldMap[cy][cx] == 3) {
                    nearExit = true;
                    
                    // If player presses F while near a chest
                    if (IsKeyPressed(KEY_F)) {
                        DrawText("PRESS [F] TO EXIT MAP", DISPLAY_SIZE/2 - 100, DISPLAY_SIZE - 100, 20, GOLD);
                    }
                }
            }
        }
    }

    // Draw the prompt if near a chest
    if (nearChest && gameState == "play") {
        DrawText("PRESS [F] TO OPEN CHEST", DISPLAY_SIZE/2 - 100, DISPLAY_SIZE - 100, 20, GOLD);
    }
}