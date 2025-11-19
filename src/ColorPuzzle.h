#ifndef COLORPUZZLE_H
#define COLORPUZZLE_H

#include "Object.h"
#include <vector>
#include <string>

enum class PuzzleColor {
    RED = 0,
    BLUE = 1,
    GREEN = 2
};

// Pushable color block
class ColorBlock : public Object {
public:
    ColorBlock(float x, float y, PuzzleColor color);
    void render() override;
    void update() override;
    
    PuzzleColor getColor() const { return color; }
    void push(float dx, float dy);
    
private:
    PuzzleColor color;
};

// Target box for color blocks
class ColorBox : public Object {
public:
    ColorBox(float x, float y, PuzzleColor targetColor);
    void render() override;
    void update() override;
    
    PuzzleColor getTargetColor() const { return targetColor; }
    bool hasCorrectBlock(const std::vector<ColorBlock*>& blocks);
    
private:
    PuzzleColor targetColor;
};

class ColorPuzzle : public Object {
public:
    ColorPuzzle(float x, float y);
    void render() override;
    void update() override;
    
    bool isSolved() const { return solved; }
    void checkSolution();
    
    std::vector<ColorBlock*>& getBlocks() { return blocks; }
    std::vector<ColorBox*>& getBoxes() { return boxes; }
    
private:
    std::vector<ColorBlock*> blocks;
    std::vector<ColorBox*> boxes;
    bool solved;
};

#endif
