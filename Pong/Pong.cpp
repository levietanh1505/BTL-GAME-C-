#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <vector>
#include <memory>
#include <string>
#include <fstream>
#include <sstream>
#include <deque>
#include "Ratio.cpp"

// Biến toàn cục lưu trữ SoundBuffers
std::vector<std::shared_ptr<sf::SoundBuffer>> soundBuffers;

// Hàm tạo âm thanh từ đường dẫn đến tệp
sf::Sound createSound(const std::string& filePath, float volume = 100.f) {
    auto buffer = std::make_shared<sf::SoundBuffer>();
    buffer->loadFromFile(filePath);
    soundBuffers.push_back(buffer); // Lưu trữ buffer để không bị hủy
    sf::Sound sound;
    sound.setBuffer(*buffer);
    sound.setVolume(volume);
    return sound;
}
// Hàm khởi tạo paddle
sf::RectangleShape createPaddle(const sf::Vector2f& size, const sf::Color& fillColor) {
    sf::RectangleShape paddle(size - sf::Vector2f(3, 3));  // Trừ đi kích thước outline
    paddle.setOutlineThickness(3);
    paddle.setOutlineColor(sf::Color::Black);
    paddle.setFillColor(fillColor);
    paddle.setOrigin(size / 2.f);  // Đặt origin ở giữa
    return paddle;
}

sf::Text createText(const std::string& text, const sf::Font& font, unsigned int characterSize, const sf::Color& color, const sf::Vector2f& position) {
    sf::Text newText;
    newText.setFont(font);
    newText.setString(text);
    newText.setCharacterSize(characterSize);
    newText.setFillColor(color);
    // Tính toán vị trí căn giữa
    sf::FloatRect textBounds = newText.getLocalBounds();
    newText.setPosition(position.x - textBounds.width / 2.f, position.y);
    return newText;
}

int main() {
    std::srand(static_cast<unsigned int>(std::time(NULL)));
    const float pi = 3.14159f;
    const int gameWidth = 1100;
    const int gameHeight = 700;
    sf::Vector2f paddleSize(25, 100);
    float ballRadius = 14.f;
    const float initialBallSpeed = 400.f;
    float ballSpeed = initialBallSpeed;

    // Khởi tạo cửa sổ game
    sf::RenderWindow window(sf::VideoMode(gameWidth, gameHeight, 32), "Game Pong", sf::Style::Titlebar | sf::Style::Close);
    window.setVerticalSyncEnabled(true);

    // Các background game
    sf::Texture backgroundTexture, backgroundTextureInGame, titleTextrue, boardTextrue;
    backgroundTexture.loadFromFile("resources/back.png");
    backgroundTextureInGame.loadFromFile("resources/back in game.png");
    titleTextrue.loadFromFile("resources/title.png");
    boardTextrue.loadFromFile("resources/boardpng.png");
    sf::Sprite titleSprite(titleTextrue);
    sf::Sprite backgroundSprite(backgroundTexture);
    sf::Sprite backgroundSpriteInGame(backgroundTextureInGame);
    sf::Sprite boardGame(boardTextrue);

    // Đặt kích thước background
    sf::Vector2u textureSize = backgroundTexture.getSize();
    backgroundSprite.setScale(static_cast<float>(gameWidth) / textureSize.x, static_cast<float>(gameHeight) / textureSize.y);
    textureSize = backgroundTextureInGame.getSize();
    backgroundSpriteInGame.setScale(static_cast<float>(gameWidth) / textureSize.x, static_cast<float>(gameHeight) / textureSize.y);
    titleSprite.setScale(0.35f, 0.15f);
    titleSprite.setPosition(190, -20);
    boardGame.setScale(0.9f, 0.9f);
    boardGame.setPosition(-30, 250);

    // Âm thanh
    sf::Sound ballSound1 = createSound("resources/paddle1.wav");
    sf::Sound ballSound2 = createSound("resources/paddle2.wav");
    sf::Sound wallSound = createSound("resources/wall.wav");
    sf::Sound soundInGame = createSound("resources/soundInGame.wav", 80.f);
    sf::Sound soundStartGame = createSound("resources/soundStartGame.wav", 90.f);
    sf::Sound soundEndGame = createSound("resources/soundEndGame.wav");
    sf::Sound soundEndGame2 = createSound("resources/soundEnd2.wav");

    // Paddle
    sf::RectangleShape leftPaddle = createPaddle(paddleSize, sf::Color::Cyan);
    sf::RectangleShape rightPaddle = createPaddle(paddleSize, sf::Color::Red);

    // Bóng đánh
    sf::CircleShape ball(ballRadius - 3);
    ball.setOutlineThickness(3);
    ball.setOutlineColor(sf::Color::Black);
    ball.setFillColor(sf::Color::Yellow);
    ball.setOrigin(ballRadius / 2, ballRadius / 2);

    Ratio ratioGame;
    std::string ratioHis = ratioGame.getRatioGame();
    // Text và font
    sf::Font font, font2;
    font.loadFromFile("resources/fontt.otf");
    font2.loadFromFile("resources/fontRatio.ttf");
    sf::Text pauseMessage = createText("Welcome to Game Pong!", font, 70, sf::Color::Green,
        sf::Vector2f(gameWidth / 2 - 0.f, 80.f));
    sf::Text startText = createText("Start", font, 50, sf::Color::Black,
        sf::Vector2f(gameWidth / 2 - 0.f, 520.f));
    sf::Text muteText = createText("Mute", font, 50, sf::Color::Black,
        sf::Vector2f(gameWidth / 2 - 0.f, 580.f));
    sf::Text muteStatus = createText("Music: on", font2, 30, sf::Color::White,
        sf::Vector2f(100.f, 15.f));
    sf::Text ratio = createText("0 - 0", font2, 80, sf::Color::Yellow,
        sf::Vector2f(gameWidth / 2 - 0.f, 150.f));
    sf::Text guide1 = createText("Left Paddle use w and a", font2, 20, sf::Color::Cyan,
        sf::Vector2f(900.f, 620.f));
    sf::Text guide2 = createText("Right Paddle use up and down", font2, 20, sf::Color::Red,
        sf::Vector2f(900.f, 650.f));
    sf::Text ratioHistory = createText(ratioHis, font2, 40, sf::Color::Red,
        sf::Vector2f(190.f, 350.f));

    // Nút bắt đầu và mute
    sf::RectangleShape startButton(sf::Vector2f(90.f, 90.f)), muteButton(sf::Vector2f(90.f, 90.f));
    startButton.setFillColor(sf::Color::Yellow);
    startButton.setPosition(gameWidth / 2 - startButton.getSize().x / 2, 500.f);
    muteButton.setFillColor(sf::Color::Yellow);
    muteButton.setPosition(gameWidth / 2 - muteButton.getSize().x / 2, 600.f);

    bool soundOn = true, isPlaying = false;
    const float paddleSpeed = 400.f;
    float ballAngle = 0.f;
    sf::Clock clock;

    int a = 0, b = 0;
    while (window.isOpen()) {
        // Phát nhạc nền khi mở game, chú ý logic phát
        if (soundOn
            && soundStartGame.getStatus() != sf::Sound::Playing
            && soundEndGame.getStatus() == sf::Sound::Stopped
            && soundEndGame2.getStatus() == sf::Sound::Stopped
            && soundInGame.getStatus() == sf::Sound::Stopped) {
            soundStartGame.setLoop(true);
            soundStartGame.play();
        }

        // Lấy sự kiện một lần duy nhất
        sf::Event event;
        while (window.pollEvent(event)) {
            if ((event.type == sf::Event::Closed) || ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Escape))) {
                window.close();
                break;
            }

            // Kiểm tra sự kiện khởi động hoặc mute
            if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Space) && !isPlaying) {
                isPlaying = true;
                clock.restart();
                leftPaddle.setPosition(20 + paddleSize.x / 2, gameHeight / 2);
                rightPaddle.setPosition(gameWidth - 20 - paddleSize.x / 2, gameHeight / 2);
                ball.setPosition(gameWidth / 2, gameHeight / 2);
                ballAngle = pi + (std::rand() % 20) * pi / 180;
                ballSpeed = initialBallSpeed;
                soundStartGame.stop();
                if (soundOn) soundInGame.play();
            }

            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                if (startButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)) && !isPlaying) {
                    isPlaying = true;
                    clock.restart();
                    leftPaddle.setPosition(20 + paddleSize.x / 2, gameHeight / 2);
                    rightPaddle.setPosition(gameWidth - 20 - paddleSize.x / 2, gameHeight / 2);
                    ball.setPosition(gameWidth / 2, gameHeight / 2);
                    ballAngle = pi + (std::rand() % 20) * pi / 180;
                    ballSpeed = initialBallSpeed;
                    soundStartGame.stop();
                    if (soundOn) soundInGame.play();
                }

                if (muteButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                    soundOn = !soundOn;
                    muteText.setString(soundOn ? "Mute" : "Unmute");
                    muteStatus.setString(soundOn ? "Music: on" : "Music: off");
                    if (!soundOn) {
                        soundStartGame.pause();
                        soundInGame.stop();
                    }
                    else if (isPlaying) {
                        soundInGame.play();
                    }
                    else {
                        soundStartGame.play();
                    }
                }
            }
        }


        window.clear();
        if (isPlaying) {
            float deltaTime = clock.restart().asSeconds();
            // Paddle trái
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)
                && (leftPaddle.getPosition().y - paddleSize.y / 2 > 8.f)) {
                leftPaddle.move(0.f, -paddleSpeed * deltaTime);
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)
                && (leftPaddle.getPosition().y + paddleSize.y / 2 < gameHeight - 5.f)) {
                leftPaddle.move(0.f, paddleSpeed * deltaTime);
            }

            // Paddle phải
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)
                && (rightPaddle.getPosition().y - paddleSize.y / 2 > 8.f)) {
                rightPaddle.move(0.f, -paddleSpeed * deltaTime);
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)
                && (rightPaddle.getPosition().y + paddleSize.y / 2 < gameHeight - 5.f)) {
                rightPaddle.move(0.f, paddleSpeed * deltaTime);
            }


            float factor = ballSpeed * deltaTime;
            ball.move(std::cos(ballAngle) * factor, std::sin(ballAngle) * factor);

            if (ball.getPosition().x - ballRadius < 0.f) {
                isPlaying = false;
                soundInGame.stop();
                if (soundOn) soundEndGame.play();
                if (soundOn && soundEndGame.getStatus() == sf::Sound::Stopped) {
                    soundStartGame.play();
                }
                b += 1;
                pauseMessage.setString("Righr Player Won!");
                pauseMessage.setPosition(sf::Vector2f(gameWidth / 2 - 190.f, 80.f));


            }
            if (ball.getPosition().x + ballRadius > gameWidth) {
                isPlaying = false;
                soundInGame.stop();
                if (soundOn) soundEndGame2.play();
                if (soundOn && soundEndGame2.getStatus() == sf::Sound::Stopped) {
                    soundStartGame.play();
                }
                a += 1;
                pauseMessage.setString("Left Player Won!");
                pauseMessage.setPosition(sf::Vector2f(gameWidth / 2 - 200.f, 80.f));
            }

            if (ball.getPosition().y - ballRadius < 0.f) {
                ballAngle = -ballAngle;
                ball.setPosition(ball.getPosition().x, ballRadius + 0.1f);
                if (soundOn) wallSound.play();

            }
            else if (ball.getPosition().y + ballRadius > gameHeight) {
                ballAngle = -ballAngle;
                ball.setPosition(ball.getPosition().x, gameHeight - ballRadius - 0.1f);
                if (soundOn) wallSound.play();
            }

            if (ball.getGlobalBounds().intersects(leftPaddle.getGlobalBounds())) {
                ballAngle = pi - ballAngle + (std::rand() % 20) * pi / 180;
                ball.setPosition(leftPaddle.getPosition().x + ballRadius + paddleSize.x / 2 + 0.1f, ball.getPosition().y);
                ballSpeed += 20.f;
                if (soundOn) ballSound1.play();
            }

            if (ball.getGlobalBounds().intersects(rightPaddle.getGlobalBounds())) {
                ballAngle = pi - ballAngle - (std::rand() % 20) * pi / 180;
                ball.setPosition(rightPaddle.getPosition().x - ballRadius - paddleSize.x / 2 - 0.1f, ball.getPosition().y);
                ballSpeed += 20.f;
                if (soundOn) ballSound2.play();
            }
        }

        // Vẽ các đối tượng
        if (isPlaying) {
            window.draw(backgroundSpriteInGame);
            window.draw(leftPaddle);
            window.draw(rightPaddle);
            window.draw(ball);
        }
        else {
            std::string res = std::to_string(a) + " - " + std::to_string(b);
            ratio.setString(res);
            window.draw(backgroundSprite);
            window.draw(titleSprite);
            window.draw(pauseMessage);
            window.draw(startButton);
            window.draw(startText);
            window.draw(muteButton);
            window.draw(muteText);
            window.draw(muteStatus);
            window.draw(ratio);
            window.draw(boardGame);
            window.draw(guide1);
            window.draw(guide2);
            window.draw(ratioHistory);

        }
        window.display();
    }

    if (!window.isOpen()) {
        if (a != 0 || b != 0) ratioGame.writeRatioToFile(a, b);
    }

    return 0;
}