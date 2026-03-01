// Update these precisely in your main.cpp
#include "raylib.h"
#include "constants.h"
#include "player.h"

#define RAYMATH_STATIC_INLINE
#include "raymath.h"  
#include "rlgl.h"

extern Player* playerObject;

const Rectangle srcFloor = { 32.0f, 240.0f, 16.0f, 16.0f }; 
const Rectangle srcTop   = { 32.0f, 74.0f,  16.0f, 16.0f };
const Rectangle srcUpDownRight { 16.0f, 74.0f,  16.0f, 16.0f };
const Rectangle srcUpDownLeft { 64.0f, 74.0f,  16.0f, 16.0f };
const Rectangle srcUpRightLeft { 48.0f, 106.0f,  16.0f, 16.0f };
const Rectangle srcDownRightLeft { 48.0f, 26.0f,  16.0f, 16.0f };
const Rectangle srcDownLeftUpRight { 48.0f, 74.0f,  16.0f, 16.0f };
const Rectangle srcFace  = { 32.0f, 74.0f,  16.0f, 16.0f }; 
const Rectangle srcFaceRight = {107.0f, 27.0f,  16.0f, 16.0f };
const Rectangle srcFaceLeft = {101.0f, 27.0f,  16.0f, 16.0f };
const Rectangle srcTopSide   = { 80.0f, 26.0f,  16.0f, 16.0f }; 
const Rectangle srcBotSide   = { 80.0f, 58.0f,  16.0f, 16.0f }; 
const Rectangle srcSide  = { 80.0f, 48.0f,  16.0f, 16.0f };  
const Rectangle srcChest = { 16.0f,  47.0f,  16.0f, 16.0f };
const Rectangle srcTopLeft = { 16.0f, 26.0f,  16.0f, 16.0f };
const Rectangle srcTopRight = { 64.0f, 26.0f,  16.0f, 16.0f };
const Rectangle srcBotRight = { 64.0f, 106.0f,  16.0f, 16.0f };
const Rectangle srcBotLeft = { 16.0f, 106.0f,  16.0f, 16.0f };

const Rectangle srcTrapDoor = {192.0f, 64.0f, 16.0f, 16.0f};


void drawTilemap(Texture2D sheet, Texture2D deco, Texture2D deco2, int worldMap[MAP_HEIGHT][MAP_WIDTH]);

void drawPlayerFOV(Vector2 playerPos, float playerAngle, int worldMap[MAP_HEIGHT][MAP_WIDTH]);

// Helper to find where one single ray hits a wall
Vector2 castSingleRay(Vector2 start, float angleRad, float maxDist, int worldMap[MAP_HEIGHT][MAP_WIDTH]);

void drawLightSystem(Vector2 playerPos, float playerAngle, RenderTexture2D lightMask, Camera2D camera, int worldMap[MAP_HEIGHT][MAP_WIDTH]);