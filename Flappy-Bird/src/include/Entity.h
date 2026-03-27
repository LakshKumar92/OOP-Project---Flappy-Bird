#pragma once
#include <SFML/Graphics.hpp>

class Entity {
public:
    virtual void update(float deltaTime) = 0; // Pure virtual function 
    virtual void draw(sf::RenderWindow& window) = 0;
    virtual ~Entity() {} 
};