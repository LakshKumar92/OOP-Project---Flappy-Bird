#include "Bird.h"
#include <iostream>

Bird::Bird() : textures(3), sprite(textures[0]) {
    // Attempt to load the image. If it fails, SFML will log an error 
    // but the game won't crash; it will just draw a white box.
    if (!textures[0].loadFromFile("Flappy-Bird/Assets/Sprites/yellowbird-upflap.png")) {
        std::cerr << "Warning: Could not load bird texture.\n";
    } else {
        if (!textures[1].loadFromFile("Flappy-Bird/Assets/Sprites/yellowbird-midflap.png") ||
            !textures[2].loadFromFile("Flappy-Bird/Assets/Sprites/yellowbird-downflap.png")) {
            std::cerr << "Warning: Could not load some bird textures.\n";
        }
        sprite.setTexture(textures[0], true); // true = reset texture rect sizes!
    }
    
    // Starting position (x, y)
    sprite.setPosition({100.f, 300.f});
    
}

void Bird::setScale(){
    // Optional: scale the bird up a bit if your image is small
    sprite.setScale({1.5f, 1.5f}); 
}

void Bird::reset() {
    sprite.setPosition({100.f, 300.f});
    velocity = 0.f;
    currentFrame = 0;
    animTimer = 0.f;
    isFlapping = false;
    if (!textures.empty()) {
        sprite.setTexture(textures[0], true); // Reset to upflap
    }
}

void Bird::flap() {
    // In SFML, the Y-axis goes DOWN. So negative velocity moves the bird UP.
    velocity = -400.f; 
    
    // Reset animation to 'upflap' (index 0) upon flapping
    if (!textures.empty()) {
        currentFrame = 0; 
        animTimer = 0.f;
        isFlapping = true;
        sprite.setTexture(textures[0], true);
    }
}

void Bird::update(float deltaTime) {
    // 1. Apply gravity to the velocity (acceleration)
    // 600.f is our gravity constant. Tweak this to make it heavier/lighter.
    velocity += 600.f * deltaTime; 
    
    // 2. Move the sprite based on the current velocity
    sprite.move({0.f, velocity * deltaTime});

    // Prevent the bird from flying above the top of the screen
    if (sprite.getPosition().y < 0.f) {
        sprite.setPosition({sprite.getPosition().x, 0.f});
        velocity = 0.f; // Stop upward momentum
    }

    // 3. Animate the bird's wings over time only if flapping
    if (!textures.empty() && isFlapping) {
        animTimer += deltaTime;
        if (animTimer > 0.1f) { // Change frame every 0.1 seconds
            animTimer = 0.f;
            currentFrame++;
            
            // The animation sequence: 0 (up), 1 (mid), 2 (down)
            int sequence[] = {0, 1, 2, 0};
            
            if (currentFrame > 3) {
                // Stop animation after one full sequence
                currentFrame = 0;
                isFlapping = false;
            }
            
            sprite.setTexture(textures[sequence[currentFrame]], true);
        }
    }
}

void Bird::draw(sf::RenderWindow& window) {
    window.draw(sprite);
}

sf::FloatRect Bird::getBounds() const {
    // This will be used later for collision detection with pipes
    return sprite.getGlobalBounds();
}