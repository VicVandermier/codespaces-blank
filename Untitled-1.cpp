#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <ctime>
#include <cstdlib>

// Cấu hình trò chơi
const int GRID_SIZE = 20;
const int GRID_WIDTH = 20;
const int GRID_HEIGHT = 20;
int speed = 100; // Tốc độ mặc định (ms)

// Cấu trúc rắn và thức ăn
struct Snake {
    std::vector<sf::Vector2i> body; // Vị trí rắn
    sf::Vector2i direction = {0, 1}; // Hướng di chuyển
};

struct Food {
    sf::Vector2i position;
};

// Sinh thức ăn ngẫu nhiên
void spawnFood(Food& food, const Snake& snake) {
    do {
        food.position = {rand() % GRID_WIDTH, rand() % GRID_HEIGHT};
    } while (std::find(snake.body.begin(), snake.body.end(), food.position) != snake.body.end());
}

// Lưu điểm cao
void saveHighScore(int score) {
    std::ofstream file("highscore.txt");
    if (file.is_open()) {
        file << score;
        file.close();
    }
}

// Đọc điểm cao
int loadHighScore() {
    std::ifstream file("highscore.txt");
    int highScore = 0;
    if (file.is_open()) {
        file >> highScore;
        file.close();
    }
    return highScore;
}

int main() {
    srand(static_cast<unsigned>(time(0)));

    // Tạo cửa sổ trò chơi
    sf::RenderWindow window(sf::VideoMode(GRID_WIDTH * GRID_SIZE, GRID_HEIGHT * GRID_SIZE + 50), "Snake Xenzia");
    window.setFramerateLimit(60);

    // Tạo rắn và thức ăn
    Snake snake;
    snake.body.push_back({5, 5});
    Food food;
    spawnFood(food, snake);

    int score = 0;
    int highScore = loadHighScore();

    // Âm thanh
    sf::SoundBuffer musicBuffer;
    if (!musicBuffer.loadFromFile("background.wav")) {
        return -1; // Kiểm tra nếu file âm thanh không tồn tại
    }
    sf::Sound music;
    music.setBuffer(musicBuffer);
    music.setLoop(true);
    music.play();

    // Font và Text
    sf::Font font;
    font.loadFromFile("arial.ttf"); // Sử dụng font Arial
    sf::Text scoreText, highScoreText;
    scoreText.setFont(font);
    highScoreText.setFont(font);
    scoreText.setCharacterSize(20);
    highScoreText.setCharacterSize(20);
    scoreText.setPosition(10, GRID_HEIGHT * GRID_SIZE);
    highScoreText.setPosition(200, GRID_HEIGHT * GRID_SIZE);

    sf::Clock clock;
    float timer = 0;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            // Điều khiển rắn
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Up && snake.direction != sf::Vector2i(0, 1)) snake.direction = {0, -1};
                if (event.key.code == sf::Keyboard::Down && snake.direction != sf::Vector2i(0, -1)) snake.direction = {0, 1};
                if (event.key.code == sf::Keyboard::Left && snake.direction != sf::Vector2i(1, 0)) snake.direction = {-1, 0};
                if (event.key.code == sf::Keyboard::Right && snake.direction != sf::Vector2i(-1, 0)) snake.direction = {1, 0};
            }
        }

        // Cập nhật trò chơi
        timer += clock.restart().asMilliseconds();
        if (timer > speed) {
            timer = 0;

            // Di chuyển rắn
            for (int i = snake.body.size() - 1; i > 0; --i) {
                snake.body[i] = snake.body[i - 1];
            }
            snake.body[0] += snake.direction;

            // Kiểm tra va chạm
            if (snake.body[0] == food.position) {
                score++;
                snake.body.push_back(snake.body.back());
                spawnFood(food, snake);
            }

            // Kiểm tra điểm cao
            if (score > highScore) {
                highScore = score;
                saveHighScore(highScore);
            }
        }

        // Vẽ giao diện
        window.clear();

        // Vẽ rắn
        sf::RectangleShape cell(sf::Vector2f(GRID_SIZE - 1, GRID_SIZE - 1));
        cell.setFillColor(sf::Color::Green);
        for (const auto& part : snake.body) {
            cell.setPosition(part.x * GRID_SIZE, part.y * GRID_SIZE);
            window.draw(cell);
        }

        // Vẽ thức ăn
        cell.setFillColor(sf::Color::Red);
        cell.setPosition(food.position.x * GRID_SIZE, food.position.y * GRID_SIZE);
        window.draw(cell);

        // Hiển thị điểm
        scoreText.setString("Score: " + std::to_string(score));
        highScoreText.setString("High Score: " + std::to_string(highScore));
        window.draw(scoreText);
        window.draw(highScoreText);

        window.display();
    }

    return 0;
}
