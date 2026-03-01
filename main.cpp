#include "raylib.h"
#include <string>
#include <iostream>
#include <vector>
#include <cmath>

// Include your other "files" (we will make these .h files so they act like JS scripts)
#include "include/inventory.h"
#include "include/player.h"
#include "include/constants.h"
#include "include/textureProcess.h"
#include "include/chest.h"

#define RAYMATH_STATIC_INLINE
#include "raymath.h"           // Include this AFTER your player header

//global inventory vars
int invIdCounter = 1;

void updateCamera(Camera2D& camera, Vector2 playerPos){
    // 1/6th of the screen is the margin
    float margin = (float)DISPLAY_SIZE / 4.5f;

    // Calculate player position relative to the camera view
    Vector2 screenPos = GetWorldToScreen2D(playerPos, camera);

    // If player hits the left 1/6th margin
    if (screenPos.x < margin) camera.target.x -= (margin - screenPos.x);
    // If player hits the right 1/6th margin
    if (screenPos.x > DISPLAY_SIZE - margin) camera.target.x += (screenPos.x - (DISPLAY_SIZE - margin));

    // Same for Y
    if (screenPos.y < margin) camera.target.y -= (margin - screenPos.y);
    if (screenPos.y > DISPLAY_SIZE - margin) camera.target.y += (screenPos.y - (DISPLAY_SIZE - margin));

    
}

void handleKeyPresses(std::string& gameState, bool& insideInventory, Selected& selected, Inventory& inv, Chest* openChest){
    if(IsKeyPressed(KEY_E)) {
        if(gameState == "play" || gameState == "inventory"){
            insideInventory = !insideInventory;
            gameState = insideInventory ? "inventory" : "play";
        }
    }

    // pressing r
    if(IsKeyPressed(KEY_R)){
        if(gameState == "inventory" && selected.active && selected.sourceInv != nullptr){
            int heldItemId = selected.id;
            Inventory& sourceInv = *selected.sourceInv;

            // first pass — just do the rotation
            for(int i = 0; i < sourceInv.invSize; i++){
                for(int j = 0; j < sourceInv.invSize; j++){
                    if(sourceInv.grid[i][j].invItemId == heldItemId){
                        sourceInv.grid[i][j].isRotated = !sourceInv.grid[i][j].isRotated;
                        int tempPosX = sourceInv.grid[i][j].position.x;
                        sourceInv.grid[i][j].position.x = sourceInv.grid[i][j].position.y;
                        sourceInv.grid[i][j].position.y = tempPosX;
                    }
                }
            }

            //find the {0,0} piece and update selected
            for(int i = 0; i < sourceInv.invSize; i++){
                for(int j = 0; j < sourceInv.invSize; j++){
                    if(sourceInv.grid[i][j].invItemId == heldItemId &&
                    sourceInv.grid[i][j].position.x == 0 &&
                    sourceInv.grid[i][j].position.y == 0){
                        selected.x = j;
                        selected.y = i;
                        break;
                    }
                }
            }
        }
    }
}

void drawUI(Player player, std::string gameState){
        //drawing text for UI
    if(gameState == "inventory"){
        DrawText("Press E to exit Inventory", 10, 10, 20, GOLD);
    }
    if(gameState == "play"){
        if (player.nearChest) {
            DrawText("PRESS [F] TO OPEN CHEST", DISPLAY_SIZE/2 - 100, DISPLAY_SIZE - 100, 20, GOLD);
        }
        else if (player.nearExit){
            DrawText("PRESS [F] TO CLIMB LADDER", DISPLAY_SIZE/2 - 100, DISPLAY_SIZE - 100, 20, GOLD);
        }
        else{
            DrawText("PRESS [E] FOR INVENTORY", DISPLAY_SIZE/2 - 100, DISPLAY_SIZE - 100, 20, GOLD);
        }
    }
}

int main() {

    // initialization and setting up local variables
    InitWindow(DISPLAY_SIZE, DISPLAY_SIZE, "Dungeon Game");
    SetTargetFPS(60);
    
    //player object
    Player player(DISPLAY_SIZE/2, DISPLAY_SIZE/2);

    //track gamestate vars, camera, map
    std::string gameState = "play";
    bool insideInventory = false;
    Camera2D camera = {0};
    int worldMap[MAP_HEIGHT][MAP_WIDTH];

    //storing chest data
    std::vector<Chest> chests;
    chests.reserve(50);
    Chest* openChest = nullptr;
        
    Texture2D dungeonSheet = LoadTexture("assets/dungeon_.png");
    Texture2D decoSheet = LoadTexture("assets/dungeonDecoration_0.png");
    Texture2D decoSheet2 = LoadTexture("assets/dungeonDecoration_1.png");

    RenderTexture2D lightMask = LoadRenderTexture(DISPLAY_SIZE, DISPLAY_SIZE);

    SetTextureFilter(dungeonSheet, TEXTURE_FILTER_POINT);
    SetTextureFilter(decoSheet, TEXTURE_FILTER_POINT);
    SetTextureFilter(decoSheet2, TEXTURE_FILTER_POINT);

    camera.target = player.pos;
    camera.offset = { (float)DISPLAY_SIZE/2, (float)DISPLAY_SIZE/2 };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    for(int i = 0; i < MAP_HEIGHT; i++){
        for(int j = 0; j< MAP_WIDTH; j++){
            worldMap[i][j] = initialMap[i][j];
            if(worldMap[i][j] == 2){
                chests.push_back({j, i, Inventory(INV_MAX_SIZE)});
            }
        }
    }

    // START DRAW
    while (!WindowShouldClose()) {
        //key pressed in loop
        handleKeyPresses(gameState, insideInventory, player.currInvSelected, player.playerInventory, openChest);

        if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if(gameState == "inventory"){
                if(openChest == nullptr) {
                    inventoryMouseHandler(player.playerInventory, player.currInvSelected, 0.0f, 1.0f);
                } else {
                    inventoryMouseHandler(player.playerInventory, player.currInvSelected, 0.0f, 0.5f);
                    inventoryMouseHandler(openChest->inventory, player.currInvSelected, (float)DISPLAY_SIZE / 2.0f, 0.5f);
                }
            } 
        }

        updateCamera(camera, player.pos);

        BeginDrawing();
            ClearBackground(BLACK);

            //if gamestate play
            if(gameState == "play") {
                openChest = nullptr;
                //begin 2dmode so camera tracks
                BeginMode2D(camera);
                //draw tilemap
                drawTilemap(dungeonSheet, decoSheet, decoSheet2, worldMap);

                //update player and draw it
                player.updatePlayer(gameState, insideInventory, worldMap, chests, openChest);
                player.drawPlayer(gameState);
                EndMode2D();

                //draw light cone logic independent of the 2dmode
                drawLightSystem(player.pos, player.playerAngle, lightMask, camera, worldMap);
                DrawTextureRec(lightMask.texture, 
                        (Rectangle){ 0, 0, (float)lightMask.texture.width, (float)-lightMask.texture.height }, 
                        (Vector2){ 0, 0 }, WHITE);

                Vector2 playerScreenPos = GetWorldToScreen2D(player.pos, camera);
            }
            //if player in inventory
            if(gameState == "inventory") {
                player.openInventory(openChest);
            }

            drawUI(player, gameState);

        EndDrawing();
    }
    // END DRAW
    UnloadTexture(dungeonSheet);
    UnloadTexture(decoSheet);
    CloseWindow();
    return 0;
}
