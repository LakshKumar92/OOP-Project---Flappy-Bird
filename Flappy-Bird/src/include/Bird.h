#include "Entity.h"
#include <vector>

class Bird : public Entity {
private:
    std::vector<sf::Texture> textures;
    sf::Sprite sprite;
    float velocity = 0;
    const float gravity = 9.8f;
    int currentFrame = 0;
    float animTimer = 0.f;
    bool isFlapping = false; // Tracks if the flap animation is playing

public:
    Bird();
    void flap();
    void update(float deltaTime) override; // Polymorphism 
    void draw(sf::RenderWindow& window) override;
    sf::FloatRect getBounds() const; // For collision detection 
    void setScale();
    void reset();
    sf::Vector2f getPosition() const { return sprite.getPosition(); }
};