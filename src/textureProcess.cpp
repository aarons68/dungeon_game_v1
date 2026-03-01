#include "../include/textureProcess.h"

#define RAYMATH_STATIC_INLINE
#include "../include/raymath.h"  
#include "../include/rlgl.h"


void drawTilemap(Texture2D sheet, Texture2D deco, Texture2D deco2, int worldMap[MAP_HEIGHT][MAP_WIDTH]) {
    auto isWall = [&](int x, int y) {
        // If outside the map, treat it as a wall to keep the perimeter solid
        if (x < 0 || x >= MAP_WIDTH || y < 0 || y >= MAP_HEIGHT) return false;
        return worldMap[y][x] == 1;
    };

    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            Rectangle dest = { (float)x * TILE_SIZE, (float)y * TILE_SIZE, (float)TILE_SIZE, (float)TILE_SIZE };

            // drwaing floor first
            DrawTexturePro(sheet, srcFloor, dest, {0,0}, 0.0f, WHITE);

            if (worldMap[y][x] == 1) {
                bool left  = isWall(x - 1, y);
                bool right = isWall(x + 1, y);
                bool up    = isWall(x, y - 1);
                bool down  = isWall(x, y + 1);

                Rectangle source = srcTop;
                Vector2 origin = { 0, 0 };

                //every case basically
                if (up && down && (!left || !right)) {
                    source = srcSide;
                }
                if( down && !up && !left && !right){
                    source = srcTopSide;
                }
                else if(!down && up && !left && !right){
                    source = srcBotSide;
                }
                else if(!down && !up && left && !right){
                    source = srcFaceRight;
                }else if(!down && !up && !left && right){
                    source = srcFaceLeft;
                }
                else if(up && down && right && left){
                    source = srcDownLeftUpRight;
                }
                else if(up && down && !right && left){
                    source = srcUpDownLeft;
                }
                else if(up && down && right && !left){
                    source = srcUpDownRight;
                }
                else if(up && !down && right && left){
                    source = srcUpRightLeft;
                }
                else if(!up && down && right && left){
                    source = srcDownRightLeft;
                }
                // --- CORNER LOGIC ---
                else if (!up && !right)  source = srcTopRight;
                else if (!up && !left) {
                    source = srcTopRight;
                    source.width *= -1;
                }
                else if (!down && !left) source = srcBotLeft;
                else if (!down && !right) {
                    source = srcBotLeft;
                    source.width *= -1;
                }

                else if (!down) {
                    source = srcFace;
                    origin.x = 0; 
                }

                if(y == 0 && x == 0){
                    source = srcTopRight;
                    source.width *= -1;
                }
                if(y == 0 && x == MAP_WIDTH-1){
                    source = srcTopRight;
                }
                if(y == MAP_HEIGHT-1 && x == 0){
                    source = srcBotLeft;
                }
                if(y == MAP_HEIGHT-1 && x == MAP_WIDTH-1){
                    source = srcBotRight;
                }

                DrawTexturePro(sheet, source, dest, origin, 0.0f, WHITE);
            } 
            else if (worldMap[y][x] == 2) {
                DrawTexturePro(deco, srcChest, dest, {0,0}, 0.0f, WHITE);
            } 
            else if (worldMap[y][x] == 3){
                DrawTexturePro(deco2, srcTrapDoor, dest, {0,0}, 0.0f, WHITE);
            }
            
        }
    }
}

void drawPlayerFOV(Vector2 playerPos, float playerAngle, int worldMap[MAP_HEIGHT][MAP_WIDTH]) {
    // 120 degree field of view
    float fovDegrees = 90.0f;
    float halfFov = fovDegrees / 2.0f;
    
    // How many rays to cast? More rays = smoother "cone"
    int numRays = 90; 
    float step = fovDegrees / (float)numRays;
    
    float maxDist = 200.0f; // Max reach of your light

    for (int i = 0; i <= numRays; i++) {
        // Calculate the current angle for this specific ray
        float currentAngle = (playerAngle - halfFov + (i * step)) * DEG2RAD;

        // Create a direction vector based on the angle
        Vector2 dir = { cosf(currentAngle), sinf(currentAngle) };
        
        // The end point starts at max distance
        Vector2 rayEnd = { 
            playerPos.x + dir.x * maxDist, 
            playerPos.y + dir.y * maxDist 
        };

        // --- COLLISION CHECK (The "Step" Method) ---
        // Incrementally move out from the player to find the first wall hit
        for (float dist = 0; dist < maxDist; dist += 4.0f) {
            float checkX = playerPos.x + dir.x * dist;
            float checkY = playerPos.y + dir.y * dist;

            int tx = (int)(checkX / TILE_SIZE);
            int ty = (int)(checkY / TILE_SIZE);

            if (tx >= 0 && tx < MAP_WIDTH && ty >= 0 && ty < MAP_HEIGHT) {
                if (worldMap[ty][tx] == 1) { // Hit a wall
                    rayEnd = { checkX, checkY };
                    break; 
                }
            }
        }

        // Draw the physical line for debugging
        DrawLineV(playerPos, rayEnd, Fade(YELLOW, 0.5f));
    }
}


// Helper to find where one single ray hits a wall
Vector2 castSingleRay(Vector2 start, float angleRad, float maxDist, int worldMap[MAP_HEIGHT][MAP_WIDTH]) {
    Vector2 dir = { cosf(angleRad), sinf(angleRad) };
    Vector2 end = { start.x + dir.x * maxDist, start.y + dir.y * maxDist };

    // Step through the map to find a wall
    for (float dist = 0; dist < maxDist; dist += 4.0f) {
        float checkX = start.x + dir.x * dist;
        float checkY = start.y + dir.y * dist;

        int tx = (int)(checkX / TILE_SIZE);
        int ty = (int)(checkY / TILE_SIZE);

        if (tx >= 0 && tx < MAP_WIDTH && ty >= 0 && ty < MAP_HEIGHT) {
            if (worldMap[ty][tx] == 1) { 
                return (Vector2){ checkX, checkY };
            }
        }
    }
    return end; // No wall hit, return the max distance point
}


void drawLightSystem(Vector2 playerPos, float playerAngle, RenderTexture2D lightMask, Camera2D camera, int worldMap[MAP_HEIGHT][MAP_WIDTH]) {
    // This is the "Offset" - it finds the difference between the world and your screen
    Vector2 offset = { 
        camera.offset.x - camera.target.x, 
        camera.offset.y - camera.target.y 
    };

    Vector2 screenPlayer = Vector2Add(playerPos, offset);
    // draw mask
    BeginTextureMode(lightMask);
        ClearBackground(Color{ 0, 0, 0, 230 }); // The "Darkness" level (0-255)

        BeginBlendMode(BLEND_CUSTOM);
        // using hex to get no errors with rlgl
        rlSetBlendFactors(0, 0x0303, 0x8006); 

            float fov = 90.0f;
            int numRays = 90; 
            float step = fov / (float)numRays;
            float maxDist = 200.0f;

            DrawCircleV(screenPlayer, 20, WHITE);

            for (int i = 0; i < numRays; i++) {
                float a1 = (playerAngle - (fov/2) + (i * step)) * DEG2RAD;
                float a2 = (playerAngle - (fov/2) + ((i + 1) * step)) * DEG2RAD;

                // cast rays into real space
                Vector2 hit1 = castSingleRay(playerPos, a1, maxDist, worldMap);
                Vector2 hit2 = castSingleRay(playerPos, a2, maxDist, worldMap);

                // shift coordinates so it looks correct
                screenPlayer = Vector2Add(playerPos, offset);
                Vector2 screenHit1 = Vector2Add(hit1, offset);
                Vector2 screenHit2 = Vector2Add(hit2, offset);

                // draw using shifted coordinates
                DrawTriangle(screenPlayer, screenHit2, screenHit1, WHITE); 
            }
                
        EndBlendMode();
    EndTextureMode();
}