#pragma once
#include "Entity.h"

class Pipe : public Entity {
private:
    sf::Sprite topPipe;
    sf::Sprite bottomPipe;
    float speed = 200.f; // How fast the pipes move left

public:
    // Constructor needs the X position, gap coords, and pre-loaded textures
    Pipe(float startX, float gapY, float gapHeight, const sf::Texture& topTex, const sf::Texture& botTex);
    
    void update(float deltaTime) override; // Polymorphism
    void draw(sf::RenderWindow& window) override;
    
    // Helper methods for game logic
    bool checkCollision(const sf::FloatRect& birdBounds) const;
    bool isOffScreen() const;

    float getRightEdge() const;
    bool isPassed = false;
};