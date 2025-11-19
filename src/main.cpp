#include "Engine.h"
#include "Object.h"
#include <iostream>
#include "ImageDevice.h"
#include <SDL_image.h>
#include "Player.h"
#include "Wall.h"
#include "Key.h"
#include "Door.h"
#include "Camera.h"
#include "ColorPuzzle.h"
#include "InputDevice.h"

//Todo: add images device
//image device will allow for the loading and retrieval of textures
// images are slow to draw...textures are faster
//Todo: add a view

//Todo: add a input device




int main(int argc, char* argv[]) 
{
   
    Engine e;
    ImageDevice::load("stone block", "assets/blocks.png", SDL_Rect{16*12, 16*5, 16, 16});
    ImageDevice::load("wood block", "assets/blocks.png", SDL_Rect{16*12, 16*1, 16, 16});
    // Load the player sprite sheet (try lowercase then capitalized filename)
    std::string playerPath;
    if (ImageDevice::load("player", "assets/player.png")) {
        playerPath = "assets/player.png";
    } else if (ImageDevice::load("player", "assets/Player.png")) {
        playerPath = "assets/Player.png";
    } else {
        std::cerr << "Warning: failed to load player sprite (tried player.png and Player.png)" << std::endl;
    }

    // If we successfully loaded the sheet, also slice it into 4 separate textures (2x2)
    if (!playerPath.empty()) {
        SDL_Surface* sheet = IMG_Load(playerPath.c_str());
        if (sheet) {
            int cols = 2, rows = 2;
            int frameW = sheet->w / cols;
            int frameH = sheet->h / rows;
            int padding = 5; // Add padding to avoid bleeding from adjacent frames
            for (int r = 0; r < rows; ++r) {
                for (int c = 0; c < cols; ++c) {
                    int idx = r * cols + c;
                    // Add padding to crop tighter and avoid seeing other frames
                    SDL_Rect src{ 
                        c * frameW + padding, 
                        r * frameH + padding, 
                        frameW - (padding * 2), 
                        frameH - (padding * 2) 
                    };
                    std::string name = "player" + std::to_string(idx);
                    ImageDevice::load(name, playerPath, src);
                }
            }
            SDL_FreeSurface(sheet);
        } else {
            std::cerr << "Warning: IMG_Load failed to open " << playerPath << ": " << IMG_GetError() << std::endl;
        }
    }
    // Create room boundaries with stone walls
    float wallThickness = 32.0f;
    float roomWidth = 800.0f;
    float roomHeight = 600.0f;
    
    // Top wall
    e.addObject(new Wall(0, 0, roomWidth, wallThickness));
    // Bottom wall
    e.addObject(new Wall(0, roomHeight - wallThickness, roomWidth, wallThickness));
    // Left wall
    e.addObject(new Wall(0, 0, wallThickness, roomHeight));
    // Right wall
    e.addObject(new Wall(roomWidth - wallThickness, 0, wallThickness, roomHeight));
    
    // Level 1 objects
    Camera* camera1 = new Camera(roomWidth - 100, 50);
    e.addObject(camera1);
    
    Key* key1 = new Key(400, 300);
    e.addObject(key1);
    
    Door* door1 = new Door(roomWidth - wallThickness - 64, roomHeight / 2 - 48, false);
    e.addObject(door1);
    
    Player* player = new Player(100, 100);
    e.addObject(player);
    
    // Level 2 objects (will be added later)
    ColorPuzzle* puzzle = nullptr;
    Key* key2 = nullptr;
    Door* door2 = nullptr;
    Camera* camera2 = nullptr;
    Camera* camera3 = nullptr;
    bool level2Started = false;
    
    Object* lastObject = e.getLastObject();
    int lastTime = SDL_GetTicks();
    double fps = 0.0;
    
    // Set static view since room fits on screen
    e.setView(0, 0);
    
    while(true)   
    {
        int frameStart = SDL_GetTicks();

        // Check for level 2 transition
        if (e.isLevel2() && !level2Started) {
            level2Started = true;
            
            // Clear all level 1 objects
            e.clearObjects();
            
            // Re-add walls
            e.addObject(new Wall(0, 0, roomWidth, wallThickness));
            e.addObject(new Wall(0, roomHeight - wallThickness, roomWidth, wallThickness));
            e.addObject(new Wall(0, 0, wallThickness, roomHeight));
            e.addObject(new Wall(roomWidth - wallThickness, 0, wallThickness, roomHeight));
            
            // Add color puzzle in center
            puzzle = new ColorPuzzle(200, 150);
            e.addObject(puzzle);
            
            // Add player back in bottom-left corner (added after puzzle so it renders on top)
            player = new Player(80, roomHeight - 150);
            e.addObject(player);
            
            // Add level 2 key (hidden until puzzle solved)
            key2 = new Key(400, 450);
            e.addObject(key2);
            
            // Add level 2 door
            door2 = new Door(roomWidth - wallThickness - 64, roomHeight / 2 - 48, true);
            e.addObject(door2);
            
            // Add two cameras for level 2
            camera2 = new Camera(100, 50);
            e.addObject(camera2);
            
            camera3 = new Camera(roomWidth - 100, roomHeight - 100);
            e.addObject(camera3);
            
            std::cout << "Level 2 started! Solve the color puzzle." << std::endl;
        }

        // Only check collisions if game is still active
        if (!e.isGameOver() && !e.isYouLose()) {
            if (!e.isLevel2()) {
                // Level 1 collisions
                player->checkCollisionWithKey(key1);
                player->checkCollisionWithDoor(door1);
                
                if (camera1->checkPlayerInVision(player)) {
                    e.showYouLose();
                    std::cout << "Caught by camera! You Lose!" << std::endl;
                }
            } else if (level2Started) {
                // Level 2 collisions
                
                // Only show key after puzzle is solved AND red block is centered
                if (puzzle && puzzle->isSolved() && key2) {
                    // Verify red block is properly centered in its box
                    auto& blocks = puzzle->getBlocks();
                    auto& boxes = puzzle->getBoxes();
                    bool redCentered = false;
                    
                    for (auto* block : blocks) {
                        if (block->getColor() == PuzzleColor::RED) {
                            for (auto* box : boxes) {
                                if (box->getTargetColor() == PuzzleColor::RED) {
                                    float blockCenterX = block->getX() + block->getWidth() / 2;
                                    float blockCenterY = block->getY() + block->getHeight() / 2;
                                    float boxCenterX = box->getX() + box->getWidth() / 2;
                                    float boxCenterY = box->getY() + box->getHeight() / 2;
                                    
                                    // Check if block is centered within 3 pixels
                                    if (abs(blockCenterX - boxCenterX) < 3 && abs(blockCenterY - boxCenterY) < 3) {
                                        redCentered = true;
                                    }
                                    break;
                                }
                            }
                            break;
                        }
                    }
                    
                    if (redCentered) {
                        player->checkCollisionWithKey(key2);
                    }
                }
                
                if (door2) {
                    player->checkCollisionWithDoor(door2);
                }
                
                // Check both cameras
                if (camera2 && camera2->checkPlayerInVision(player)) {
                    e.showYouLose();
                    std::cout << "Caught by TOP-LEFT camera! Player at (" << player->getX() << ", " << player->getY() << ") Camera at (" << camera2->getX() << ", " << camera2->getY() << ")" << std::endl;
                }
                if (camera3 && camera3->checkPlayerInVision(player)) {
                    e.showYouLose();
                    std::cout << "Caught by BOTTOM-RIGHT camera! Player at (" << player->getX() << ", " << player->getY() << ") Camera at (" << camera3->getX() << ", " << camera3->getY() << ")" << std::endl;
                }
                
                // Check for block pickup/drop system
                if (puzzle && !puzzle->isSolved()) {
                    auto& blocks = puzzle->getBlocks();
                    auto& boxes = puzzle->getBoxes();
                    
                    // If player is carrying a block, move it with them
                    if (player->getCarriedBlock()) {
                        ColorBlock* carriedBlock = player->getCarriedBlock();
                        // Position block at player's position (slightly offset)
                        carriedBlock->setX(player->getX() + 6);
                        carriedBlock->setY(player->getY() - 5);
                        
                        // Check if player is over the correct box
                        for (auto* box : boxes) {
                            if (carriedBlock->getColor() == box->getTargetColor()) {
                                float playerCenterX = player->getX() + player->getWidth() / 2;
                                float playerCenterY = player->getY() + player->getHeight() / 2;
                                
                                // Check if player center is inside the box
                                if (playerCenterX >= box->getX() && playerCenterX <= box->getX() + box->getWidth() &&
                                    playerCenterY >= box->getY() && playerCenterY <= box->getY() + box->getHeight()) {
                                    // Snap block to center of box and release it
                                    float targetX = box->getX() + (box->getWidth() - carriedBlock->getWidth()) / 2;
                                    float targetY = box->getY() + (box->getHeight() - carriedBlock->getHeight()) / 2;
                                    carriedBlock->setX(targetX);
                                    carriedBlock->setY(targetY);
                                    player->setCarriedBlock(nullptr);
                                    std::cout << "Block locked in place!" << std::endl;
                                    break;
                                }
                            }
                        }
                    } else {
                        // Not carrying anything, check if player touches a block to pick it up
                        for (auto* block : blocks) {
                            // Skip blocks that are already correctly placed
                            bool isPlaced = false;
                            for (auto* box : boxes) {
                                if (block->getColor() == box->getTargetColor()) {
                                    float blockCenterX = block->getX() + block->getWidth() / 2;
                                    float blockCenterY = block->getY() + block->getHeight() / 2;
                                    float boxCenterX = box->getX() + box->getWidth() / 2;
                                    float boxCenterY = box->getY() + box->getHeight() / 2;
                                    
                                    if (abs(blockCenterX - boxCenterX) < 5 && abs(blockCenterY - boxCenterY) < 5) {
                                        isPlaced = true;
                                        break;
                                    }
                                }
                            }
                            
                            if (!isPlaced) {
                                // Check if player is touching this block
                                if (player->getX() < block->getX() + block->getWidth() &&
                                    player->getX() + player->getWidth() > block->getX() &&
                                    player->getY() < block->getY() + block->getHeight() &&
                                    player->getY() + player->getHeight() > block->getY()) {
                                    // Pick up the block
                                    player->setCarriedBlock(block);
                                    std::cout << "Picked up block!" << std::endl;
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }
        
        e.update();

        int currentTime = SDL_GetTicks();
     
        fps = 1000.0f / (currentTime - lastTime);
        lastTime = currentTime;
        std::cout << "FPS: " << fps << std::endl;
        // std::cout << "Frame Time: " << (currentTime-frameStart) << std::endl;
        SDL_Delay((1000/200) - (currentTime-frameStart)/1000);

    }
    return 0;
}
