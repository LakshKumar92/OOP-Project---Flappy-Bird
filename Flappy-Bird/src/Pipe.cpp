#include "Pipe.h"
#include <iostream>

Pipe::Pipe(float startX, float gapY, float gapHeight, const sf::Texture& topTex, const sf::Texture& botTex) 
    : topPipe(topTex), bottomPipe(botTex) {

    // Scale them to look thick enough (1.5x of 52px = 78px width, 480px height)
    topPipe.setScale({1.5f, 1.5f});
    bottomPipe.setScale({1.5f, 1.5f});

    // Top pipe should have its bottom edge right at gapY. 
    // Since scaled height is 320 * 1.5 = 480, we move it UP by 480 from gapY.
    topPipe.setPosition({startX, gapY - 480.f});
    
    // Bottom pipe should have its top edge right at gapY + gapHeight
    float bottomPipeY = gapY + gapHeight;
    bottomPipe.setPosition({startX, bottomPipeY});
}

void Pipe::update(float deltaTime) {
    // SFML 3 requires a Vector2f for move operations
    topPipe.move({-speed * deltaTime, 0.f});
    bottomPipe.move({-speed * deltaTime, 0.f});
}

void Pipe::draw(sf::RenderWindow& window) {
    window.draw(topPipe);
    window.draw(bottomPipe);
}

bool Pipe::checkCollision(const sf::FloatRect& birdBounds) const {
    // SFML 3 uses findIntersection which returns an std::optional
    return topPipe.getGlobalBounds().findIntersection(birdBounds).has_value() || 
           bottomPipe.getGlobalBounds().findIntersection(birdBounds).has_value();
}

bool Pipe::isOffScreen() const {
    // SFML 3 uses .position.x and GlobalBounds size
    return topPipe.getPosition().x + topPipe.getGlobalBounds().size.x < 0;
}

float Pipe::getRightEdge() const {
    return topPipe.getPosition().x + topPipe.getGlobalBounds().size.x;
}
