#include "raylib.h"
#include <string>
#include <iostream>
#include <vector>
#include <cmath>

// Include your other "files" (we will make these .h files so they act like JS scripts)
#include "inventory.h"
#include "player.h"
#include "constants.h"
#include "textureProcess.h"

#define RAYMATH_STATIC_INLINE
#include "raymath.h"           // Include this AFTER your player header


// global playerObject
Player* playerObject;

// global gameState
std::string gameState = "play";

//global inventory vars
bool insideInventory = false;
int invIdCounter = 0;


//global camera 
Camera2D camera = { 0 };

// START SETUP
void setup() {
    // draw player in middle of the screen
    playerObject = new Player(DISPLAY_SIZE/2, DISPLAY_SIZE/2);

    camera.target = playerObject->pos;
    camera.offset = { (float)DISPLAY_SIZE/2, (float)DISPLAY_SIZE/2 };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;
}
// END SETUP

void updateCamera(){
    // 1/6th of the screen is the margin
    float margin = (float)DISPLAY_SIZE / 4.5f;

    // Calculate player position relative to the camera view
    Vector2 screenPos = GetWorldToScreen2D(playerObject->pos, camera);

    // If player hits the left 1/6th margin
    if (screenPos.x < margin) camera.target.x -= (margin - screenPos.x);
    // If player hits the right 1/6th margin
    if (screenPos.x > DISPLAY_SIZE - margin) camera.target.x += (screenPos.x - (DISPLAY_SIZE - margin));

    // Same for Y
    if (screenPos.y < margin) camera.target.y -= (margin - screenPos.y);
    if (screenPos.y > DISPLAY_SIZE - margin) camera.target.y += (screenPos.y - (DISPLAY_SIZE - margin));
}

void handleKeyPresses(){
    if(IsKeyPressed(KEY_E)) {
        if(gameState == "play" || gameState == "inventory"){
        insideInventory = !insideInventory;
        gameState = insideInventory ? "inventory" : "play";
        }
    }

    // pressing r
    if(IsKeyPressed(KEY_R)){
        // if game is in inventory and holding an object
        if(gameState == "inventory" && playerObject->currInvSelected.active){
            
            int heldItemId = playerObject->currInvSelected.id;
            int invSize = playerObject->playerInventory.invSize;

            // loop to find all item ID that match
            for(int i = 0; i < invSize; i++){
                for(int j = 0; j < invSize; j++){
                
                    if(playerObject->playerInventory.grid[i][j].invItemId == heldItemId){
                        //make them all rotate
                        playerObject->playerInventory.grid[i][j].isRotated = !playerObject->playerInventory.grid[i][j].isRotated;

                        //swap x and y pos 
                        int tempPosX = playerObject->playerInventory.grid[i][j].position.x;
                        playerObject->playerInventory.grid[i][j].position.x = playerObject->playerInventory.grid[i][j].position.y;
                        playerObject->playerInventory.grid[i][j].position.y = tempPosX;
                    }
                }
            }
        }
    }
}

int main() {
    InitWindow(DISPLAY_SIZE, DISPLAY_SIZE, "Dungeon Game");
    SetTargetFPS(60);
        
    Texture2D dungeonSheet = LoadTexture("assets/dungeon_.png");
    Texture2D decoSheet = LoadTexture("assets/dungeonDecoration_0.png");
    Texture2D decoSheet2 = LoadTexture("assets/dungeonDecoration_1.png");

    RenderTexture2D lightMask = LoadRenderTexture(DISPLAY_SIZE, DISPLAY_SIZE);

    if (dungeonSheet.id == 0) std::cout << "ERROR: Could not load dungeon_.png" << std::endl;
    if (decoSheet.id == 0) std::cout << "ERROR: Could not load dungeonDecoration_0.png" << std::endl;

    SetTextureFilter(dungeonSheet, TEXTURE_FILTER_POINT);
    SetTextureFilter(decoSheet, TEXTURE_FILTER_POINT);
    SetTextureFilter(decoSheet2, TEXTURE_FILTER_POINT);

    setup();

    // START DRAW
    while (!WindowShouldClose()) {
        // Handle keyPressed logic inside the loop
        handleKeyPresses();

        if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if(gameState == "inventory") inventoryMouseHandler();
        }

        updateCamera();

        BeginDrawing();
            ClearBackground(BLACK);
            if(gameState == "play") {
                BeginMode2D(camera);
                drawTilemap(dungeonSheet, decoSheet, decoSheet2);
                playerObject->updatePlayer();
                playerObject->drawPlayer();
                EndMode2D();
                drawLightSystem(playerObject->playerAngle, lightMask, camera);
                DrawTextureRec(lightMask.texture, 
                        (Rectangle){ 0, 0, (float)lightMask.texture.width, (float)-lightMask.texture.height }, 
                        (Vector2){ 0, 0 }, WHITE);

                Vector2 playerScreenPos = GetWorldToScreen2D(playerObject->pos, camera);
            }
            if(gameState == "inventory") {
                playerObject->openInventory();
            }
            DrawText("Press E for Inventory", 10, 10, 20, GOLD);

        EndDrawing();

    }
    // END DRAW
    UnloadTexture(dungeonSheet);
    UnloadTexture(decoSheet);
    CloseWindow();
    return 0;
}

        