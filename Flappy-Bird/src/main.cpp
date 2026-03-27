#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "Bird.h"
#include "Pipe.h"
#include <vector>
#include <cstdlib>
#include <ctime>
#include <string>

int main() {
    std::srand(static_cast<unsigned>(std::time(nullptr))); // Initialize random seed

    // Create the game window (Width: 400, Height: 600)
    sf::RenderWindow window(sf::VideoMode({400, 600}), "Flappy Bird");
    window.setFramerateLimit(60); // Cap at 60 FPS

    // Set the window icon (Title bar and Taskbar)
    sf::Image icon;
    if (icon.loadFromFile("Flappy-Bird/Assets/favicon.png")) {
        window.setIcon(icon.getSize(), icon.getPixelsPtr());
    }

    // Instantiate our Bird object
    Bird player;
    
    // --- SETUP BACKGROUND ---
    sf::Texture backgroundTexture;
    if (!backgroundTexture.loadFromFile("Flappy-Bird/Assets/Sprites/background-day.png")) {
        // Will print an error in console automatically if it fails
    }
    sf::Sprite backgroundSprite(backgroundTexture);
    backgroundSprite.scale({1.5f, 1.5f});
    backgroundSprite.setPosition({0.f, -150.f}); // Move background a little up

    sf::Texture baseTexture;
    baseTexture.setRepeated(true); // Allow texture to map beyond its bounds
    baseTexture.loadFromFile("Flappy-Bird/Assets/Sprites/base.png");
    sf::Sprite basesprite(baseTexture);
    
    // We make the sprite much wider than the screen so we can scroll its texture mapping
    basesprite.setTextureRect({{0, 0}, {800, (int)baseTexture.getSize().y}});
    basesprite.scale({1.5f, 1.5f}); // Scale to match background
    basesprite.setPosition({0.f, 600.f - (90.f * 1.5f)}); // Position at the bottom

    float baseScrollOffset = 0.f;
    const float baseScrollSpeed = 150.f; // Pixels per second

    sf::Texture gameOverTexture;
    if (!gameOverTexture.loadFromFile("Flappy-Bird/Assets/Sprites/gameover.png")) {
        // error handling
    }
    sf::Sprite gameOverSprite(gameOverTexture);
    gameOverSprite.setPosition({
        400.f / 2.f - gameOverTexture.getSize().x / 2.f, 
        600.f / 2.f - gameOverTexture.getSize().y / 2.f
    });

    // --- PIPES TEXTURES ---
    sf::Texture topPipeTex;
    topPipeTex.loadFromFile("Flappy-Bird/Assets/Sprites/pipedown.png"); // The one pointing downwards
    sf::Texture botPipeTex;
    botPipeTex.loadFromFile("Flappy-Bird/Assets/Sprites/pipe.png"); // The one pointing upwards

    // --- START SCREEN TEXTURE ---
    sf::Texture startTexture;
    startTexture.loadFromFile("Flappy-Bird/Assets/Sprites/message.png");
    sf::Sprite startSprite(startTexture);
    startSprite.scale({1.5f, 1.5f});
    startSprite.setPosition({
        400.f / 2.f - (startTexture.getSize().x * 1.5f) / 2.f, 
        600.f / 2.f - (startTexture.getSize().y * 1.5f) / 2.f - 50.f
    });

    // --- SCORE TEXTURES ---
    std::vector<sf::Texture> numberTextures(10);
    for (int i = 0; i <= 9; ++i) {
        numberTextures[i].loadFromFile("Flappy-Bird/Assets/Sprites/" + std::to_string(i) + ".png");
    }
    int score = 0;

    // --- SOUND EFFECTS ---
    sf::SoundBuffer wingBuffer;
    wingBuffer.loadFromFile("Flappy-Bird/Assets/Audio/wing.wav");
    sf::Sound wingSound(wingBuffer);

    sf::SoundBuffer pointBuffer;
    pointBuffer.loadFromFile("Flappy-Bird/Assets/Audio/point.wav");
    sf::Sound pointSound(pointBuffer);

    sf::SoundBuffer hitBuffer;
    hitBuffer.loadFromFile("Flappy-Bird/Assets/Audio/hit.wav");
    sf::Sound hitSound(hitBuffer);

    sf::SoundBuffer swooshBuffer;
    swooshBuffer.loadFromFile("Flappy-Bird/Assets/Audio/swoosh.wav");
    sf::Sound swooshSound(swooshBuffer);

    // Clock for delta time calculations
    sf::Clock clock;

    bool isGameOver = false;
    bool gameStart = false;

    // Pipes container
    std::vector<Pipe> pipes;
    float pipeSpawnTimer = 0.f;
    const float pipeSpawnInterval = 1.5f; // spawn a pipe every 1.5 seconds

    // --- MAIN GAME LOOP ---
    while (window.isOpen()) {
        // Get time elapsed since last frame
        float deltaTime = clock.restart().asSeconds();

        // Handle events (keyboard input, closing window)
        while (const auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }

            // Flap when Spacebar is pressed!
            if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                if (keyPressed->code == sf::Keyboard::Key::Space) {
                    if (!isGameOver && gameStart) {
                        player.flap();
                        wingSound.play();
                    } else if(!gameStart) {
                        gameStart = true;
                        player.flap();
                        wingSound.play();
                    }
                    else {
                        // Reset the game if space is pressed during Game Over
                        swooshSound.play(); // Play swoosh sound on reset
                        isGameOver = false;
                        // gameStart = false; // Go back to start screen
                        score = 0; // Reset score
                        // To properly restart, we just need to reposition the bird and wipe velocity (requires a small method in Bird)
                        player.reset();
                        pipes.clear();
                        pipeSpawnTimer = 0.f;
                    }
                }
            }
        }

        if (!isGameOver && gameStart) {
            // Update game logic (Polymorphism in action if we had a list of Entities)
            player.update(deltaTime);

            // Pipe Spawning and Updating
            pipeSpawnTimer += deltaTime;
            if (pipeSpawnTimer >= pipeSpawnInterval) {
                pipeSpawnTimer = 0.f;
                // random gap position between 100 and 350
                float gapY = 50.f + static_cast<float>(std::rand() % 200); 
                pipes.emplace_back(400.f, gapY, 175.f, topPipeTex, botPipeTex); // start X=400, gapHeight=150, pass textures!
            }

            for (auto it = pipes.begin(); it != pipes.end(); ) {
                it->update(deltaTime);
                
                // Check if bird passed the pipe
                if (!it->isPassed && player.getPosition().x > it->getRightEdge()) {
                    score++;
                    pointSound.play();
                    it->isPassed = true;
                }

                if (it->isOffScreen()) {
                    it = pipes.erase(it);
                } else {
                    ++it;
                }
            }

            // Check collision with pipes
            for (const auto& pipe : pipes) {
                if (pipe.checkCollision(player.getBounds())) {
                    if (!isGameOver) hitSound.play();
                    isGameOver = true;
                }
            }

            // Check for collision with the ground
            // Ground Y position is basesprite.getPosition().y
            // Bird bottom Y position is player.getPosition().y + player.getBounds().size.y
            if (player.getPosition().y + player.getBounds().size.y >= basesprite.getPosition().y) {
                if (!isGameOver) hitSound.play();
                isGameOver = true;
            }
        }

        // Scroll the ground even before the game starts (but stop if game over)
        if (!isGameOver) {
            baseScrollOffset += baseScrollSpeed * deltaTime;
            // The base texture is 336 pixels wide. Reset offset before it wraps completely
            if (baseScrollOffset > 336.f) {
                baseScrollOffset -= 336.f;
            }
            basesprite.setTextureRect({{(int)baseScrollOffset, 0}, {800, (int)baseTexture.getSize().y}});
        }

        // --- RENDERING ---
        // 1. Clear the screen (optional when drawing a background that covers the whole screen)
        window.clear();

        // 2. Draw the background FIRST so it sits behind the bird
        window.draw(backgroundSprite);

        // 2.5 Draw Pipes
        for (auto& pipe : pipes) {
            pipe.draw(window);
        }

        // Draw Base over pipes
        window.draw(basesprite);

        // 3. Draw the bird
        player.setScale();
        if(gameStart) player.draw(window);

        // Draw Score
        if (gameStart) {
            // 1. Convert the integer score to a string (e.g., if score is 12, string is "12")
            std::string scoreStr = std::to_string(score);
            std::vector<sf::Sprite> scoreSprites;
            // 2. Loop through every single character in that string
            for (char c : scoreStr) {
                // 3. Convert the character back to an integer (e.g., '1' becomes the number 1)
                int digit = c - '0';
                // 4. Look up the texture array using that specific digit
                sf::Sprite digitSprite(numberTextures[digit]);
                digitSprite.setScale({1.5f, 1.5f});
                // 5. Add it to our list of sprites to draw
                scoreSprites.push_back(digitSprite);
            }
            float startX = 20.f; // 20px padding from the left edge
            float currentX = startX;
            for (auto& spr : scoreSprites) {
                spr.setPosition({currentX, 20.f}); // Draw near the top-left
                window.draw(spr);
                currentX += spr.getGlobalBounds().size.x + 2.f;
            }
        }

        // Draw Game Start / Game Over overlays
        if (!gameStart) {
            window.draw(startSprite);
        } else if (isGameOver) {
            window.draw(gameOverSprite);
        }

        // 4. Display what was drawn
        window.display();
    }

    return 0;
}
