#include "ColorPuzzle.h"
#include "Engine.h"
#include <iostream>

ColorBlock::ColorBlock(float x, float y, PuzzleColor color)
    : Object(x, y, 40, 40), color(color)
{
}

void ColorBlock::render()
{
    int r = 0, g = 0, b = 0;
    switch(color) {
        case PuzzleColor::RED:   r = 255; g = 0;   b = 0;   break;
        case PuzzleColor::BLUE:  r = 0;   g = 100; b = 255; break;
        case PuzzleColor::GREEN: r = 0;   g = 255; b = 0;   break;
    }
    
    // Draw the block
    Engine::drawRect(getX(), getY(), getWidth(), getHeight(), r, g, b, 255);
    // Draw border
    Engine::drawRect(getX(), getY(), getWidth(), 2, 0, 0, 0, 255);
    Engine::drawRect(getX(), getY(), 2, getHeight(), 0, 0, 0, 255);
    Engine::drawRect(getX() + getWidth() - 2, getY(), 2, getHeight(), 0, 0, 0, 255);
    Engine::drawRect(getX(), getY() + getHeight() - 2, getWidth(), 2, 0, 0, 0, 255);
}

void ColorBlock::update()
{
}

void ColorBlock::push(float dx, float dy)
{
    setX(getX() + dx);
    setY(getY() + dy);
}

ColorBox::ColorBox(float x, float y, PuzzleColor targetColor)
    : Object(x, y, 50, 50), targetColor(targetColor)
{
}

void ColorBox::render()
{
    int r = 0, g = 0, b = 0;
    switch(targetColor) {
        case PuzzleColor::RED:   r = 200; g = 100; b = 100; break;
        case PuzzleColor::BLUE:  r = 100; g = 150; b = 200; break;
        case PuzzleColor::GREEN: r = 100; g = 200; b = 100; break;
    }
    
    // Draw box with dashed border pattern
    Engine::drawRect(getX(), getY(), getWidth(), getHeight(), r, g, b, 150);
    
    // Draw thick border
    Engine::drawRect(getX(), getY(), getWidth(), 4, 50, 50, 50, 255);
    Engine::drawRect(getX(), getY(), 4, getHeight(), 50, 50, 50, 255);
    Engine::drawRect(getX() + getWidth() - 4, getY(), 4, getHeight(), 50, 50, 50, 255);
    Engine::drawRect(getX(), getY() + getHeight() - 4, getWidth(), 4, 50, 50, 50, 255);
}

void ColorBox::update()
{
}

bool ColorBox::hasCorrectBlock(const std::vector<ColorBlock*>& blocks)
{
    // Check if any block of matching color is centered in this box
    for (auto* block : blocks) {
        if (block->getColor() == targetColor) {
            float blockCenterX = block->getX() + block->getWidth() / 2;
            float blockCenterY = block->getY() + block->getHeight() / 2;
            
            if (blockCenterX >= getX() && blockCenterX <= getX() + getWidth() &&
                blockCenterY >= getY() && blockCenterY <= getY() + getHeight()) {
                return true;
            }
        }
    }
    return false;
}

ColorPuzzle::ColorPuzzle(float x, float y)
    : Object(x, y, 400, 300), solved(false)
{
    // Create 3 target boxes at the top
    boxes.push_back(new ColorBox(x + 50, y + 50, PuzzleColor::RED));
    boxes.push_back(new ColorBox(x + 175, y + 50, PuzzleColor::BLUE));
    boxes.push_back(new ColorBox(x + 300, y + 50, PuzzleColor::GREEN));
    
    // Create 3 pushable blocks at the bottom (shuffled positions)
    blocks.push_back(new ColorBlock(x + 180, y + 240, PuzzleColor::GREEN));
    blocks.push_back(new ColorBlock(x + 55, y + 240, PuzzleColor::BLUE));
    blocks.push_back(new ColorBlock(x + 305, y + 240, PuzzleColor::RED));
}

void ColorPuzzle::render()
{
    // Draw background panel
    Engine::drawRect(getX(), getY(), getWidth(), getHeight(), 80, 80, 80, 255);
    
    // Draw instruction text
    if (!solved) {
        Engine::drawText("Push blocks into matching boxes", getX() + 30, getY() + 10, 20, 255, 255, 255, 255);
    } else {
        Engine::drawText("SOLVED! Get the key!", getX() + 80, getY() + 10, 24, 0, 255, 0, 255);
    }
    
    // Render boxes first (so blocks appear on top)
    for (auto* box : boxes) {
        box->render();
    }
    
    // Render blocks
    for (auto* block : blocks) {
        block->render();
    }
}

void ColorPuzzle::update()
{
    if (!solved) {
        checkSolution();
    }
}

void ColorPuzzle::checkSolution()
{
    // Check if all boxes have their correct colored block
    for (auto* box : boxes) {
        if (!box->hasCorrectBlock(blocks)) {
            return; // Not solved yet
        }
    }
    
    // All boxes have correct blocks
    solved = true;
    std::cout << "Puzzle solved!" << std::endl;
}
