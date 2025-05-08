#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <ctime>
#include <algorithm>

// Constants for game setup
const unsigned int WINDOW_WIDTH = 1000;
const unsigned int WINDOW_HEIGHT = 700;
const std::string GAME_TITLE = "Code Blaster";

// assets locations 
const std::string FONT_PATH = "C:/Users/HP/Desktop/Sfml project/resources/sansation.ttf";
const std::string CORRECT_SOUND_PATH = "C:/Users/HP/Desktop/Code game OOps/Assests/explosion-42132.wav";
const std::string INCORRECT_SOUND_PATH = "C:/Users/HP/Desktop/Code game OOps/Assests/buzzer-or-wrong-answer-20582 (1).wav";
const std::string MUSIC_PATH = "C:/Users/HP/Desktop/Code game OOps/Assests/the-world-of-8-bit-games-301273.wav";
const std::string EXPLOSION_TEXTURE_PATH = "C:/Users/HP/Desktop/Code game OOps/Assests/explosion2.png";
const std::string BACKGROUND_TEXTURE_PATH = "C:/Users/HP/Desktop/Code game OOps/Assests/Blue Green Pixel Retro Quiz Trivia Game Presentation.png";
const std::string HOME_IMAGE_PATH = "C:/Users/HP/Desktop/Code game OOps/Assests/spaceMeteors_002.png";
const std::string SHOOTER_TEXTURE_PATH = "C:/Users/HP/Desktop/Code game OOps/Assests/spaceRockets_001.png";
const std::string BULLET_TEXTURE_PATH = "C:/Users/HP/Desktop/Code game OOps/Assests/spaceMissiles_009.png";

// --- Bullet Class ---
// Represents a bullet shot by the shooter
class Bullet {
public:
    sf::Sprite sprite;
    float speed;

    Bullet(const sf::Texture& texture, float speed_) : speed(speed_) {
        sprite.setTexture(texture);
        sprite.setOrigin(texture.getSize().x / 2.f, texture.getSize().y / 2.f);
    }

    void update() {
        sprite.move(0.f, -speed); // Move bullet upward
    }

    bool isOffScreen() const {
        return sprite.getPosition().y < 0; // Off screen if above window
    }

    void setPosition(const sf::Vector2f& position) {
        sprite.setPosition(position);
    }
};

// --- Shooter Class ---
// Player's shooter entity with bullet firing capability
class Shooter {
public:
    sf::Sprite sprite;
    sf::Texture texture;
    sf::Texture bulletTexture;
    std::vector<Bullet> bullets;
    float bulletSpeed;

    Shooter() : bulletSpeed(10.f) {}

    // hooter and bullet loaded textures from files
    bool loadTextures(const std::string& shooterPath, const std::string& bulletPath) {
        if (!texture.loadFromFile(shooterPath)) {
            std::cerr << "Error loading shooter texture: " << shooterPath << std::endl;
            return false;
        }
        if (!bulletTexture.loadFromFile(bulletPath)) {
            std::cerr << "Error loading bullet texture: " << bulletPath << std::endl;
            return false;
        }
        sprite.setTexture(texture);
        sprite.setOrigin(texture.getSize().x / 2.f, texture.getSize().y / 2.f);
        sprite.setPosition(WINDOW_WIDTH / 2.f, WINDOW_HEIGHT - 100);
        return true;
    }

    // Create a new bullet at the tip of the shooter
    void shoot() {
        Bullet bullet(bulletTexture, bulletSpeed);
        bullet.setPosition({ sprite.getPosition().x, sprite.getPosition().y - sprite.getGlobalBounds().height / 2.f });
        bullets.push_back(bullet);
    }

    // Update bullets' position and remove off-screen bullets
    void update() {
        for (auto& bullet : bullets)
            bullet.update();
        bullets.erase(std::remove_if(bullets.begin(), bullets.end(),
            [](const Bullet& b) { return b.isOffScreen(); }), bullets.end());
    }

    // Draw shooter and all bullets
    void draw(sf::RenderWindow& window) {
        window.draw(sprite);
        for (const auto& bullet : bullets)
            window.draw(bullet.sprite);
    }
};

// --- Button Class ---
// UI Button with support for multi-line text and hover/disabled states
class Button {
public:
    sf::RectangleShape rect;
    std::vector<sf::Text> textLines;
    sf::Font& font;
    unsigned int charSize;
    sf::Color idleColor = sf::Color(70, 70, 150);
    sf::Color hoverColor = sf::Color(120, 120, 220);
    sf::Color textColor = sf::Color::White;
    sf::Color disabledColor = sf::Color(50, 50, 100);
    sf::Color disabledTextColor = sf::Color(110, 110, 160);
    bool enabled = true;

    // Constructor to create button with size, position, text, font and character size
    Button(sf::Vector2f size, sf::Vector2f position, const std::string& buttonText, sf::Font& btnFont, unsigned int charSize_ = 20)
        : font(btnFont), charSize(charSize_)
    {
        rect.setSize(size);
        rect.setFillColor(idleColor);
        rect.setPosition(position);
        rect.setOrigin(size.x / 2.f, size.y / 2.f);

        // Split multi-line text by lines and create sf::Text for each
        std::istringstream iss(buttonText);
        std::string line;
        float lineHeight = charSize * 1.3f;
        std::vector<sf::Text> tempLines;
        float totalHeight = 0.f;

        while (std::getline(iss, line)) {
            sf::Text textLine;
            textLine.setFont(font);
            textLine.setString(line);
            textLine.setCharacterSize(charSize);
            textLine.setFillColor(textColor);
            sf::FloatRect bounds = textLine.getLocalBounds();
            textLine.setOrigin(bounds.left + bounds.width / 2.f, bounds.top + bounds.height / 2.f);
            tempLines.push_back(textLine);
            totalHeight += lineHeight;
        }

        // Position each text line vertically centered in button
        float startY = position.y - totalHeight / 2.f + lineHeight / 2.f;
        for (unsigned i = 0; i < tempLines.size(); ++i) {
            tempLines[i].setPosition(position.x, startY + i * lineHeight);
            textLines.push_back(tempLines[i]);
        }
    }

    // Update button color based on mouse position and enabled state
    void updateColor(const sf::Vector2i& mousePos) {
        if (!enabled) {
            rect.setFillColor(disabledColor);
            for (auto& t : textLines) t.setFillColor(disabledTextColor);
        }
        else if (isMouseOver(mousePos)) {
            rect.setFillColor(hoverColor);
            for (auto& t : textLines) t.setFillColor(textColor);
        }
        else {
            rect.setFillColor(idleColor);
            for (auto& t : textLines) t.setFillColor(textColor);
        }
    }

    // Check if mouse is over button (only if enabled)
    bool isMouseOver(const sf::Vector2i& mousePos) const {
        sf::Vector2f mf(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
        return enabled && rect.getGlobalBounds().contains(mf);
    }

    // Draw button rectangle and text lines
    void draw(sf::RenderWindow& window) const {
        window.draw(rect);
        for (const auto& t : textLines) window.draw(t);
    }

    // Enable or disable button and update color accordingly
    void setEnabled(bool val) {
        enabled = val;
        updateColor({ -1, -1 }); // Mouse position invalid, just refresh color
    }
};

// --- Question Struct ---
// Represents a multiple-choice question for a level
struct Question {
    std::string questionText;
    std::vector<std::string> options;
    int correctOptionIndex;

    Question() : correctOptionIndex(0) {}
    Question(const std::string& q, const std::vector<std::string>& opts, int correct)
        : questionText(q), options(opts), correctOptionIndex(correct) {
    }
};

// --- Level Struct ---
// Represents a level that contains questions
struct Level {
    std::string levelName;
    std::vector<Question> questions;
    Level() {}
    Level(const std::string& name, const std::vector<Question>& qs) : levelName(name), questions(qs) {}
};

// --- Explosion Class ---
// Handles animated explosion sprite for incorrect answers
class Explosion {
public:
    sf::Sprite sprite;
    sf::Texture texture;
    int frameWidth = 0;
    int frameHeight = 0;
    int numFrames = 0;
    int currentFrame = 0;
    float frameDuration = 0.05f;
    float timer = 0.f;
    bool isPlaying = false;
    sf::Clock clock;

    Explosion() : currentFrame(0), timer(0.f), isPlaying(false) {}

    // Load explosion spritesheet texture and calculate frames
    bool loadTexture(const std::string& path, int frames) {
        if (!texture.loadFromFile(path)) {
            std::cerr << "Error loading explosion texture: " << path << std::endl;
            return false;
        }
        numFrames = frames;
        frameWidth = texture.getSize().x / numFrames;
        frameHeight = texture.getSize().y;
        sprite.setTexture(texture);
        sprite.setTextureRect(sf::IntRect(0, 0, frameWidth, frameHeight));
        sprite.setOrigin(frameWidth / 2.f, frameHeight / 2.f);
        return true;
    }

    // Start animation at given position
    void start(sf::Vector2f position) {
        if (frameWidth <= 0) return;
        sprite.setPosition(position);
        currentFrame = 0;
        isPlaying = true;
        sprite.setTextureRect(sf::IntRect(0, 0, frameWidth, frameHeight));
        clock.restart();
    }

    // Update animation frames based on time elapsed
    void update() {
        if (!isPlaying) return;

        timer += clock.restart().asSeconds();

        while (timer >= frameDuration && isPlaying) {
            timer -= frameDuration;
            currentFrame++;
            if (currentFrame >= numFrames) {
                isPlaying = false;
                currentFrame = 0;
                sprite.setTextureRect(sf::IntRect(0, 0, frameWidth, frameHeight));
            }
            else {
                sprite.setTextureRect(sf::IntRect(currentFrame * frameWidth, 0, frameWidth, frameHeight));
            }
        }
    }

    // Draw the explosion if playing
    void draw(sf::RenderWindow& window) {
        if (isPlaying)
            window.draw(sprite);
    }
};

// --- Game Class ---
// Core game class handling game states, events, rendering, and logic
class Game {
private:
    sf::RenderWindow window;
    sf::Font font;
    sf::Music backgroundMusic;
    sf::SoundBuffer correctSoundBuffer;
    sf::SoundBuffer incorrectSoundBuffer;
    sf::Sound correctSound;
    sf::Sound incorrectSound;

    sf::Texture backgroundTexture;
    sf::Sprite backgroundSprite;

    sf::Texture homeImageTexture;
    sf::Sprite homeImageSprite;

    Shooter shooter;

    // Enumeration of game states
    enum GameState { MENU, LEVEL_SELECTION, RULES, IN_GAME, GAME_OVER };
    GameState gameState;

    std::vector<Level> levels;
    int currentLevel;
    int currentQuestionIndex;
    int score;
    int lives;

    // Menu Elements
    Button* startButton;
    Button* exitButton;
    Button* rulesButton;
    sf::Text gameNameText;

    // Level Selection Elements
    std::vector<Button> levelButtons;
    sf::Text levelSelectText;
    Button* backToMenuFromLevelSelectButton;
    std::vector<bool> levelUnlocked;
    sf::Text escapeHintText;

    // Rules Screen Elements
    sf::Text rulesTitleText;
    sf::Text rulesBodyText;
    sf::RectangleShape rulesBackgroundRect; // Background rectangle for rules text
    Button* backToMenuFromRulesButton;

    // In-Game Elements
    sf::Text questionText;
    std::vector<sf::Text> optionTexts;
    std::vector<sf::RectangleShape> optionBoxes;
    int selectedOption;
    sf::RectangleShape selectionIndicator;
    sf::Text scoreTextBox;
    sf::RectangleShape scoreBox;
    sf::Text livesText;
    sf::Text levelProgressText;
    sf::Text feedbackText;
    bool showingFeedback;
    sf::Clock feedbackTimer;
    Explosion explosion;

    // Game Over Elements
    sf::Text gameOverText;
    sf::Text finalScoreText;
    Button* backToMenuFromGameOverButton;

public:
    Game();
    ~Game();
    void run();

private:
    void loadResources();
    void loadLevels();
    void setupCommonText();
    void setupMenu();
    void setupLevelSelection();
    void setupRules();
    void setupQuestionDisplay();
    void setupGameOver();
    void loadQuestion(int levelIdx, int questionIdx);
    void updateScoreText();
    void updateLivesText();
    void processEvents();
    void processMenuEvents(const sf::Event& event);
    void processLevelSelectionEvents(const sf::Event& event);
    void processRulesEvents(const sf::Event& event);
    void processInGameEvents(const sf::Event& event);
    void processGameOverEvents(const sf::Event& event);
    void update();
    void updateSelectionIndicator();
    void checkAnswer(int selected);
    void render();
};

// Constructor definition
Game::Game()
    : window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), GAME_TITLE, sf::Style::Close | sf::Style::Titlebar),
    currentLevel(0), currentQuestionIndex(0), score(0), lives(3), selectedOption(-1), showingFeedback(false),
    startButton(nullptr), exitButton(nullptr), rulesButton(nullptr),
    backToMenuFromLevelSelectButton(nullptr), backToMenuFromRulesButton(nullptr), backToMenuFromGameOverButton(nullptr)
{
    window.setFramerateLimit(60);
    srand(static_cast<unsigned>(time(nullptr)));

    if (!font.loadFromFile(FONT_PATH)) {
        std::cerr << "FATAL ERROR: Could not load font from " << FONT_PATH << "\n";
        window.close();
        return;
    }

    float centerX = WINDOW_WIDTH / 2.f;

    // Initialize buttons for menu and screens
    startButton = new Button({ 200, 60 }, { centerX, 250.f }, "Start Game", font, 30);
    rulesButton = new Button({ 150, 60 }, { centerX, 340.f }, "Rules", font, 30);
    exitButton = new Button({ 140, 60 }, { centerX, 430.f }, "Exit", font, 30);
    backToMenuFromLevelSelectButton = new Button({ 180, 40 }, { 100.f, WINDOW_HEIGHT - 50.f }, "Back to Menu", font, 20);
    backToMenuFromRulesButton = new Button({ 180, 40 }, { 100.f, WINDOW_HEIGHT - 50.f }, "Back to Menu", font, 20);
    backToMenuFromGameOverButton = new Button({ 220, 50 }, { centerX, WINDOW_HEIGHT * 0.75f }, "Back to Menu", font, 24);

    loadResources();
    loadLevels();

    setupCommonText();
    setupMenu();
    setupLevelSelection();
    setupRules();
    setupQuestionDisplay();
    setupGameOver();

    if (backgroundMusic.openFromFile(MUSIC_PATH)) {
        backgroundMusic.setLoop(true);
        backgroundMusic.setVolume(40.f);
        backgroundMusic.play();
    }
}

// Destructor definition
Game::~Game() {
    delete startButton;
    delete rulesButton;
    delete exitButton;
    delete backToMenuFromLevelSelectButton;
    delete backToMenuFromRulesButton;
    delete backToMenuFromGameOverButton;
}

// Load game resources
void Game::loadResources() {
    if (!correctSoundBuffer.loadFromFile(CORRECT_SOUND_PATH))
        std::cerr << "Warning loading correct sound.\n";
    else
        correctSound.setBuffer(correctSoundBuffer);

    if (!incorrectSoundBuffer.loadFromFile(INCORRECT_SOUND_PATH))
        std::cerr << "Warning loading incorrect sound.\n";
    else
        incorrectSound.setBuffer(incorrectSoundBuffer);

    if (!backgroundTexture.loadFromFile(BACKGROUND_TEXTURE_PATH))
        std::cerr << "Warning loading background texture.\n";
    else {
        backgroundSprite.setTexture(backgroundTexture);
        backgroundSprite.setScale(
            float(WINDOW_WIDTH) / backgroundTexture.getSize().x,
            float(WINDOW_HEIGHT) / backgroundTexture.getSize().y);
    }

    if (!homeImageTexture.loadFromFile(HOME_IMAGE_PATH)) {
        std::cerr << "Warning loading home page image.\n";
    }
    else {
        homeImageSprite.setTexture(homeImageTexture);
        float scaleX = 300.f / homeImageTexture.getSize().x;
        float scaleY = 150.f / homeImageTexture.getSize().y;
        float scale = std::min(scaleX, scaleY);
        homeImageSprite.setScale(scale, scale);
        homeImageSprite.setPosition(WINDOW_WIDTH / 2.f - (homeImageTexture.getSize().x * scale) / 2.f, 180.f);
    }

    if (!explosion.loadTexture(EXPLOSION_TEXTURE_PATH, 5))
        std::cerr << "Warning loading explosion texture.\n";

    if (!shooter.loadTextures(SHOOTER_TEXTURE_PATH, BULLET_TEXTURE_PATH))
        std::cerr << "Warning loading shooter or bullet textures.\n";
}

// Load levels and questions, initialize unlock vector
void Game::loadLevels() {
    levels.clear();

    levels.emplace_back("Lesson: Introduction\nto Variables", std::vector<Question>{
        Question("What is a variable?", { "A place to store data", "A type of operator", "A data structure", "None of the above" }, 0),
            Question("Which of the following is an integer?", { "5", "5.5", "Hello", "True" }, 0),
            Question("Which is not a valid variable type in C++?", { "int", "float", "char", "number" }, 3),
            Question("Which of the following is used for declaring a variable?", { "int", "printf", "cin", "return" }, 0),
            Question("What is the default value of an uninitialized int in C++?", { "0", "undefined", "garbage", "null" }, 2)
    });

    levels.emplace_back("Lesson: Functions\nand Default Args", std::vector<Question>{
        Question("What is a function?", { "A block of code that performs a specific task", "A variable", "A loop", "None of the above" }, 0),
            Question("Can functions have default arguments?", { "Yes", "No", "Only if they are pointers", "None of the above" }, 0),
            Question("How do you declare a function in C++?", { "void myFunction()", "function myFunction()", "def myFunction()", "None of the above" }, 0),
            Question("What does 'return' do in a function?", { "Exits the function and optionally returns a value", "Executes a loop", "Ends the program", "None of the above" }, 0),
            Question("What is a function prototype?", { "A declaration of a function before its actual definition", "A loop", "A variable", "None of the above" }, 0)
    });

    levels.emplace_back("Lesson: Object-Oriented\nProgramming Basics", std::vector<Question>{
        Question("What is polymorphism?", { "Ability to change shape", "Ability to inherit", "Ability to access memory", "None of the above" }, 0),
            Question("What is a class in C++?", { "A template for objects", "A function", "A variable", "None of the above" }, 0),
            Question("What does inheritance allow?", { "A class to inherit properties from another class", "A function to return values", "A variable to store data", "None of the above" }, 0),
            Question("What is a constructor in C++?", { "A special function to initialize objects", "A function to destroy objects", "A function to add data", "None of the above" }, 0),
            Question("What is the difference between a constructor and a destructor?", { "A constructor initializes, a destructor cleans up", "A constructor is optional", "A destructor is used for loops", "None of the above" }, 0)
    });

    levels.emplace_back("Lesson: Loops\nand Iteration", std::vector<Question>{
        Question("What is a 'for' loop?", { "A control flow statement for iteration", "An if statement", "A function", "None of the above" }, 0),
            Question("What does 'break' do in a loop?", { "Exit the loop immediately", "Continue to next iteration", "Start a new loop", "None of the above" }, 0),
            Question("What is the difference between 'while' and 'do-while' loops?", { "'do-while' executes at least once", "'while' executes at least once", "No difference", "None of the above" }, 0),
            Question("What keyword is used to skip current iteration?", { "continue", "break", "skip", "pass" }, 0),
            Question("Can loops be nested?", { "Yes", "No", "Sometimes", "None of the above" }, 0)
    });

    levels.emplace_back("Lesson: Arrays\nand Vectors", std::vector<Question>{
        Question("What is an array?", { "A collection of elements", "A variable", "A function", "None of the above" }, 0),
            Question("What is 'std::vector'?", { "A dynamic array", "A fixed array", "A linked list", "None of the above" }, 0),
            Question("How do you access array elements?", { "Using indices starting at zero", "Using keys", "By value", "None of the above" }, 0),
            Question("Can vectors resize dynamically?", { "Yes", "No", "Only in C++11", "None of the above" }, 0),
            Question("What header is needed for std::vector?", { "#include <vector>", "#include <array>", "#include <list>", "None of the above" }, 0)
    });

    levels.emplace_back("Lesson: Pointers\nand Memory", std::vector<Question>{
        Question("What is a pointer?", { "A variable storing memory address", "An integer", "A function", "None of the above" }, 0),
            Question("What does 'nullptr' represent?", { "Null pointer", "Zero", "Uninitialized pointer", "None of the above" }, 0),
            Question("What operator dereferences a pointer?", { "*", "&", "->", "None of the above" }, 0),
            Question("What does 'new' keyword do?", { "Allocates memory", "Deletes memory", "Creates pointer", "None of the above" }, 0),
            Question("What is a smart pointer?", { "Manages lifetime of dynamically allocated memory", "Regular pointer", "Array", "None of the above" }, 0)
    });

    levelUnlocked.assign(levels.size(), false);
    if (!levelUnlocked.empty())
        levelUnlocked[0] = true;
}

// Setup fonts, texts and UI components common to all screens
void Game::setupCommonText() {
    scoreBox.setSize({ 120.f, 40.f });
    scoreBox.setFillColor(sf::Color(30, 30, 70, 200));
    scoreBox.setOutlineColor(sf::Color::Cyan);
    scoreBox.setOutlineThickness(3.f);
    scoreBox.setPosition(WINDOW_WIDTH - 140.f, 10.f);

    scoreTextBox.setFont(font);
    scoreTextBox.setCharacterSize(22);
    scoreTextBox.setFillColor(sf::Color::Cyan);
    scoreTextBox.setPosition(scoreBox.getPosition().x + 10.f, scoreBox.getPosition().y + 5.f);

    livesText.setFont(font);
    livesText.setCharacterSize(24);
    livesText.setFillColor(sf::Color::White);
    livesText.setPosition(WINDOW_WIDTH - 120, 60);

    levelProgressText.setFont(font);
    levelProgressText.setCharacterSize(20);
    levelProgressText.setFillColor(sf::Color(200, 200, 200));

    feedbackText.setFont(font);
    feedbackText.setCharacterSize(48);

    escapeHintText.setFont(font);
    escapeHintText.setCharacterSize(14);
    escapeHintText.setFillColor(sf::Color(180, 180, 180));
}

// Setup main menu with centered buttons
void Game::setupMenu() {
    gameNameText.setFont(font);
    gameNameText.setString(GAME_TITLE);
    gameNameText.setCharacterSize(60);
    gameNameText.setFillColor(sf::Color::Cyan);
    sf::FloatRect titleBounds = gameNameText.getLocalBounds();
    gameNameText.setOrigin(titleBounds.left + titleBounds.width / 2.f, titleBounds.top + titleBounds.height / 2.f);
    gameNameText.setPosition(WINDOW_WIDTH / 2.f, 120);

    startButton->updateColor({ -1, -1 });
    rulesButton->updateColor({ -1, -1 });
    exitButton->updateColor({ -1, -1 });

    escapeHintText.setString("(Esc to quit)");
    sf::FloatRect hintBounds = escapeHintText.getLocalBounds();
    escapeHintText.setOrigin(hintBounds.left + hintBounds.width / 2.f, hintBounds.top + hintBounds.height / 2.f);
    escapeHintText.setPosition(WINDOW_WIDTH / 2.f, WINDOW_HEIGHT - 20.f);
}

// Setup level selection screen with unlock logic and big multi-line buttons
void Game::setupLevelSelection() {
    levelSelectText.setFont(font);
    levelSelectText.setString("Select Level");
    levelSelectText.setCharacterSize(40);
    levelSelectText.setFillColor(sf::Color::Yellow);
    sf::FloatRect lsBounds = levelSelectText.getLocalBounds();

    levelSelectText.setOrigin(lsBounds.left + lsBounds.width / 2.f, lsBounds.top + lsBounds.height / 2.f);
    levelSelectText.setPosition(WINDOW_WIDTH / 2.f, 40);  // Moved upward from 60 to 40 for spacing

    levelButtons.clear();

    const float buttonWidth = 700.f;
    const float totalAvailableHeight = WINDOW_HEIGHT - 150.f;
    const float verticalSpacing = 12.f;
    float buttonHeight = (totalAvailableHeight - (verticalSpacing * (levels.size() - 1))) / levels.size();
    if (buttonHeight > 100.f)
        buttonHeight = 100.f;

    const float startX = WINDOW_WIDTH / 2.f;
    const float startY = 100.f; // Adjusted downward slightly for button start to fit after header

    for (size_t i = 0; i < levels.size(); ++i) {
        float posY = startY + static_cast<float>(i) * (buttonHeight + verticalSpacing);
        levelButtons.emplace_back(
            sf::Vector2f(buttonWidth, buttonHeight),
            sf::Vector2f(startX, posY),
            levels[i].levelName,
            font,
            24
        );
        levelButtons.back().setEnabled(i < levelUnlocked.size() ? levelUnlocked[i] : false);
    }

    backToMenuFromLevelSelectButton->updateColor({ -1, -1 });

    sf::FloatRect backBounds = backToMenuFromLevelSelectButton->rect.getGlobalBounds();
    escapeHintText.setString("(Esc to go back)");
    escapeHintText.setCharacterSize(14);
    escapeHintText.setFillColor(sf::Color(180, 180, 180));
    escapeHintText.setOrigin(0, escapeHintText.getLocalBounds().height / 2.f);
    escapeHintText.setPosition(backBounds.left + backBounds.width + 10.f, backBounds.top + backBounds.height / 2.f);
}

// Setup rules screen with semi-transparent background behind the rules text only
void Game::setupRules() {
    rulesTitleText.setFont(font);
    rulesTitleText.setString("Game Rules");
    rulesTitleText.setCharacterSize(40);
    rulesTitleText.setFillColor(sf::Color::Yellow);
    sf::FloatRect rtBounds = rulesTitleText.getLocalBounds();
    rulesTitleText.setOrigin(rtBounds.left + rtBounds.width / 2.f, rtBounds.top + rtBounds.height / 2.f);
    rulesTitleText.setPosition(WINDOW_WIDTH / 2.f, 100);

    rulesBodyText.setFont(font);
    rulesBodyText.setString(
        "Welcome to the Cosmic Quiz Challenge!\n\n"
        "- Select a level to begin.\n"
        "- Answer multiple-choice questions about C++ programming.\n"
        "- Click on the answer you think is correct.\n"
        "- You start with 3 lives. An incorrect answer costs one life.\n"
        "- Complete all questions in a level to unlock the next one.\n"
        "- Your score increases with each correct answer.\n"
        "- Have fun!\n\n"
        "Good luck, Cadet!"
    );
    rulesBodyText.setCharacterSize(20);
    rulesBodyText.setFillColor(sf::Color::White);
    rulesBodyText.setPosition(100.f, 180.f);

    // Calculate the bounding rect of rulesBodyText for background rectangle
    sf::FloatRect rulesBounds = rulesBodyText.getLocalBounds();
    rulesBackgroundRect.setSize(sf::Vector2f(rulesBounds.width + 40.f, rulesBounds.height + 30.f));
    rulesBackgroundRect.setFillColor(sf::Color(0, 0, 0, 180)); // semi-transparent black
    rulesBackgroundRect.setPosition(rulesBodyText.getPosition().x - 20.f, rulesBodyText.getPosition().y - 10.f);

    backToMenuFromRulesButton->updateColor({ -1, -1 });

    sf::FloatRect backBounds = backToMenuFromRulesButton->rect.getGlobalBounds();
    escapeHintText.setString("(Esc to go back)");
    escapeHintText.setCharacterSize(14);
    escapeHintText.setFillColor(sf::Color(180, 180, 180));
    escapeHintText.setOrigin(0, escapeHintText.getLocalBounds().height / 2.f);
    escapeHintText.setPosition(backBounds.left + backBounds.width + 10.f, backBounds.top + backBounds.height / 2.f);
}

// Setup question display for in-game screen
void Game::setupQuestionDisplay() {
    questionText.setFont(font);
    questionText.setCharacterSize(28);
    questionText.setFillColor(sf::Color::White);

    optionTexts.resize(4);
    optionBoxes.resize(4);
    float startY = 180.f;
    float spacing = 80.f;
    float optionWidth = WINDOW_WIDTH - 100.f;
    float optionHeight = 60.f;

    for (int i = 0; i < 4; i++) {
        optionBoxes[i].setSize({ optionWidth, optionHeight });
        optionBoxes[i].setFillColor(sf::Color(90, 90, 150));
        optionBoxes[i].setOutlineColor(sf::Color(160, 160, 255));
        optionBoxes[i].setOutlineThickness(3.f);
        optionBoxes[i].setPosition(50.f, startY + i * spacing);

        optionTexts[i].setFont(font);
        optionTexts[i].setCharacterSize(22);
        optionTexts[i].setFillColor(sf::Color::White);
    }

    selectionIndicator.setSize({ optionWidth + 10, optionHeight + 10 });
    selectionIndicator.setFillColor(sf::Color::Transparent);
    selectionIndicator.setOutlineColor(sf::Color::Yellow);
    selectionIndicator.setOutlineThickness(4.f);
    selectedOption = -1;
}

// Setup game over screen elements
void Game::setupGameOver() {
    gameOverText.setFont(font);
    gameOverText.setString("GAME OVER");
    gameOverText.setCharacterSize(72);
    gameOverText.setFillColor(sf::Color::Red);
    sf::FloatRect goBounds = gameOverText.getLocalBounds();
    gameOverText.setOrigin(goBounds.left + goBounds.width / 2.f, goBounds.top + goBounds.height / 2.f);
    gameOverText.setPosition(WINDOW_WIDTH / 2.f, WINDOW_HEIGHT / 3.f);

    finalScoreText.setFont(font);
    finalScoreText.setString("Final Score: " + std::to_string(score));
    finalScoreText.setCharacterSize(36);
    finalScoreText.setFillColor(sf::Color::White);
    sf::FloatRect fsBounds = finalScoreText.getLocalBounds();
    finalScoreText.setOrigin(fsBounds.left + fsBounds.width / 2.f, fsBounds.top + fsBounds.height / 2.f);
    finalScoreText.setPosition(WINDOW_WIDTH / 2.f, WINDOW_HEIGHT / 2.f);

    backToMenuFromGameOverButton->updateColor({ -1, -1 });

    sf::FloatRect backBounds = backToMenuFromGameOverButton->rect.getGlobalBounds();
    escapeHintText.setString("(Esc to go back)");
    escapeHintText.setCharacterSize(14);
    escapeHintText.setFillColor(sf::Color(180, 180, 180));
    escapeHintText.setOrigin(0, escapeHintText.getLocalBounds().height / 2.f);
    escapeHintText.setPosition(backBounds.left + backBounds.width + 10.f, backBounds.top + backBounds.height / 2.f);
}

// Load question data for current level/question
void Game::loadQuestion(int levelIdx, int questionIdx) {
    if ((unsigned)levelIdx >= levels.size() || (unsigned)questionIdx >= levels[levelIdx].questions.size()) {
        gameState = LEVEL_SELECTION;
        setupLevelSelection();
        return;
    }
    const Question& q = levels[levelIdx].questions[questionIdx];
    questionText.setString("Q" + std::to_string(questionIdx + 1) + ": " + q.questionText);
    sf::FloatRect qBounds = questionText.getLocalBounds();
    questionText.setOrigin(qBounds.left + qBounds.width / 2.f, qBounds.top + qBounds.height / 2.f);
    questionText.setPosition(WINDOW_WIDTH / 2.f, 100.f);

    for (int i = 0; i < 4; i++) {
        if ((unsigned)i < q.options.size()) {
            char optionLabel = 'A' + i;
            optionTexts[i].setString(std::string(1, optionLabel) + ") " + q.options[i]);
        }
        else optionTexts[i].setString("");
        sf::FloatRect optBounds = optionTexts[i].getLocalBounds();
        optionTexts[i].setOrigin(optBounds.left, optBounds.top + optBounds.height / 2.f);
        optionTexts[i].setPosition(optionBoxes[i].getPosition().x + 20.f, optionBoxes[i].getPosition().y + optionBoxes[i].getSize().y / 2.f);
    }
    selectedOption = -1;
    showingFeedback = false;

    levelProgressText.setString("Question " + std::to_string(questionIdx + 1) + " / " + std::to_string(levels[levelIdx].questions.size()));
    sf::FloatRect progBounds = levelProgressText.getLocalBounds();
    levelProgressText.setOrigin(progBounds.left + progBounds.width / 2.f, progBounds.top + progBounds.height / 2.f);
    levelProgressText.setPosition(WINDOW_WIDTH / 2.f, 35.f);
}

// Update displayed score text
void Game::updateScoreText() {
    scoreTextBox.setString("Score: " + std::to_string(score));
}

// Update displayed lives text
void Game::updateLivesText() {
    livesText.setString("Lives: " + std::to_string(lives));
}

// Process SFML events
void Game::processEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            window.close();

        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
            if (gameState == IN_GAME || gameState == LEVEL_SELECTION || gameState == RULES || gameState == GAME_OVER) {
                gameState = MENU;
                setupMenu();
                if (backgroundMusic.getStatus() == sf::Music::Paused)
                    backgroundMusic.play();
                return;
            }
            else if (gameState == MENU) {
                window.close();
            }
        }

        switch (gameState) {
        case MENU: processMenuEvents(event); break;
        case LEVEL_SELECTION: processLevelSelectionEvents(event); break;
        case RULES: processRulesEvents(event); break;
        case IN_GAME: if (!showingFeedback) processInGameEvents(event); break;
        case GAME_OVER: processGameOverEvents(event); break;
        }
    }
}

// Process menu events (buttons click)
void Game::processMenuEvents(const sf::Event& event) {
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    startButton->updateColor(mousePos);
    rulesButton->updateColor(mousePos);
    exitButton->updateColor(mousePos);
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        if (startButton->isMouseOver(mousePos)) {
            gameState = LEVEL_SELECTION;
            setupLevelSelection();
        }
        else if (rulesButton->isMouseOver(mousePos)) {
            gameState = RULES;
            setupRules();
        }
        else if (exitButton->isMouseOver(mousePos)) {
            window.close();
        }
    }
}

// Level selection screen click handling
void Game::processLevelSelectionEvents(const sf::Event& event) {
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    for (auto& btn : levelButtons) btn.updateColor(mousePos);
    backToMenuFromLevelSelectButton->updateColor(mousePos);
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        for (size_t i = 0; i < levelButtons.size(); ++i) {
            if (levelButtons[i].enabled && levelButtons[i].isMouseOver(mousePos)) {
                currentLevel = static_cast<int>(i);
                currentQuestionIndex = 0;
                score = 0;
                lives = 3;
                gameState = IN_GAME;
                loadQuestion(currentLevel, currentQuestionIndex);
                updateScoreText();
                updateLivesText();
                return;
            }
        }
        if (backToMenuFromLevelSelectButton->isMouseOver(mousePos)) {
            gameState = MENU;
            setupMenu();
        }
    }
}

// Rules screen event handling
void Game::processRulesEvents(const sf::Event& event) {
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    backToMenuFromRulesButton->updateColor(mousePos);
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        if (backToMenuFromRulesButton->isMouseOver(mousePos)) {
            gameState = MENU;
            setupMenu();
        }
    }
}

// In-game events including option selection
void Game::processInGameEvents(const sf::Event& event) {
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    sf::Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
    selectedOption = -1;
    for (size_t i = 0; i < optionBoxes.size(); ++i) {
        if (optionBoxes[i].getGlobalBounds().contains(mousePosF)) {
            selectedOption = static_cast<int>(i);
            break;
        }
    }
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        if (selectedOption != -1) {
            checkAnswer(selectedOption);
        }
    }
}

// Game over event handling
void Game::processGameOverEvents(const sf::Event& event) {
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    backToMenuFromGameOverButton->updateColor(mousePos);
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        if (backToMenuFromGameOverButton->isMouseOver(mousePos)) {
            gameState = MENU;
            setupMenu();
            if (backgroundMusic.getStatus() != sf::Music::Playing) {
                backgroundMusic.play();
            }
        }
    }
}

// Main update loop for animations and game state updates
void Game::update() {
    explosion.update();
    shooter.update();
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);

    switch (gameState) {
    case MENU:
        startButton->updateColor(mousePos);
        rulesButton->updateColor(mousePos);
        exitButton->updateColor(mousePos);
        break;
    case LEVEL_SELECTION:
        for (auto& btn : levelButtons) btn.updateColor(mousePos);
        backToMenuFromLevelSelectButton->updateColor(mousePos);
        break;
    case RULES:
        backToMenuFromRulesButton->updateColor(mousePos);
        break;
    case IN_GAME:
        if (showingFeedback) {
            if (feedbackTimer.getElapsedTime().asSeconds() > 1.5f) {
                showingFeedback = false;
                // Unlock next level only if player still has lives
                size_t uLevel = static_cast<size_t>(currentLevel);
                if (uLevel < levels.size()) {
                    size_t nextQ = static_cast<size_t>(currentQuestionIndex) + 1;
                    if (nextQ < levels[uLevel].questions.size()) {
                        currentQuestionIndex++;
                        loadQuestion(currentLevel, currentQuestionIndex);
                    }
                    else {
                        // Completed current level; unlock next only if lives > 0
                        if (lives > 0) {
                            size_t nextL = uLevel + 1;
                            if (nextL < levels.size() && nextL < levelUnlocked.size()) {
                                levelUnlocked[nextL] = true;
                            }
                        }
                        gameState = LEVEL_SELECTION;
                        setupLevelSelection();
                        if (backgroundMusic.getStatus() == sf::Music::Paused)
                            backgroundMusic.play();
                    }
                }
                else {
                    gameState = MENU;
                    setupMenu();
                }
            }
        }
        else {
            updateSelectionIndicator();
        }
        break;
    case GAME_OVER:
        backToMenuFromGameOverButton->updateColor(mousePos);
        break;
    }
}

// Update visual indicator for selected option in question
void Game::updateSelectionIndicator() {
    if (selectedOption >= 0 && static_cast<size_t>(selectedOption) < optionBoxes.size()) {
        selectionIndicator.setPosition(optionBoxes[selectedOption].getPosition() + sf::Vector2f(-5.f, -5.f));
    }
    else {
        selectionIndicator.setPosition(-200.f, -200.f); // Move off-screen if none selected
    }
}

// Check if selected answer is correct; update lives, score, and provide feedback
void Game::checkAnswer(int selected) {
    size_t uLevel = static_cast<size_t>(currentLevel);
    size_t uQuestion = static_cast<size_t>(currentQuestionIndex);

    if (uLevel >= levels.size() || uQuestion >= levels[uLevel].questions.size()) {
        std::cerr << "Error: Invalid indices in checkAnswer." << std::endl;
        return;
    }

    showingFeedback = true;
    feedbackTimer.restart();
    const Question& question = levels[uLevel].questions[uQuestion];

    if (selected == question.correctOptionIndex) {
        feedbackText.setString("Correct!");
        feedbackText.setFillColor(sf::Color::Green);
        score += 10;
        correctSound.play();
        updateScoreText();
        shooter.shoot();
    }
    else {
        feedbackText.setString("Incorrect!");
        feedbackText.setFillColor(sf::Color::Red);
        lives--;
        incorrectSound.play();
        updateLivesText();
        if (static_cast<size_t>(selected) < optionBoxes.size()) {
            sf::Vector2f boxPos = optionBoxes[selected].getPosition();
            sf::Vector2f boxSize = optionBoxes[selected].getSize();
            explosion.start(boxPos + boxSize / 2.f);
        }
        if (lives <= 0) {
            // Show game over with final score
            gameState = GAME_OVER;
            setupGameOver();
            if (backgroundMusic.getStatus() == sf::Music::Playing) {
                backgroundMusic.stop();
            }
            return;
        }
    }
    sf::FloatRect fbBounds = feedbackText.getLocalBounds();
    feedbackText.setOrigin(fbBounds.left + fbBounds.width / 2.f, fbBounds.top + fbBounds.height / 2.f);
    feedbackText.setPosition(WINDOW_WIDTH / 2.f, WINDOW_HEIGHT / 2.f - 50.f);
}


// Render all UI elements based on current game state
void Game::render() {
    window.clear(sf::Color(10, 10, 30));
    window.draw(backgroundSprite);

    switch (gameState) {
    case MENU:
        window.draw(gameNameText);
        window.draw(homeImageSprite);
        startButton->draw(window);
        rulesButton->draw(window);
        exitButton->draw(window);

        escapeHintText.setString("(Esc to quit)");
        escapeHintText.setCharacterSize(14);
        escapeHintText.setFillColor(sf::Color(180, 180, 180));
        {
            sf::FloatRect hintBounds = escapeHintText.getLocalBounds();
            escapeHintText.setOrigin(hintBounds.left + hintBounds.width / 2.f, hintBounds.top + hintBounds.height / 2.f);
            escapeHintText.setPosition(WINDOW_WIDTH / 2.f, WINDOW_HEIGHT - 20.f);
        }
        window.draw(escapeHintText);
        break;

    case LEVEL_SELECTION:
        window.draw(levelSelectText);
        for (auto& btn : levelButtons) btn.draw(window);
        backToMenuFromLevelSelectButton->draw(window);
        window.draw(escapeHintText);
        break;

    case RULES:
        window.draw(rulesTitleText);
        window.draw(rulesBackgroundRect); // Draw background behind rules text only
        window.draw(rulesBodyText);
        backToMenuFromRulesButton->draw(window);
        window.draw(escapeHintText);
        break;

    case IN_GAME:
        window.draw(questionText);
        for (size_t i = 0; i < optionBoxes.size(); ++i) {
            window.draw(optionBoxes[i]);
            window.draw(optionTexts[i]);
        }
        if (selectedOption != -1 && !showingFeedback)
            window.draw(selectionIndicator);
        // Draw score box and score text
        window.draw(scoreBox);
        window.draw(scoreTextBox);
        window.draw(livesText);
        window.draw(levelProgressText);
        if (showingFeedback)
            window.draw(feedbackText);
        explosion.draw(window);
        shooter.draw(window);

        escapeHintText.setString("(Esc to go back)");
        escapeHintText.setCharacterSize(14);
        escapeHintText.setFillColor(sf::Color(180, 180, 180));
        escapeHintText.setOrigin(0, escapeHintText.getLocalBounds().height / 2.f);
        escapeHintText.setPosition(10.f, WINDOW_HEIGHT - 20.f);
        window.draw(escapeHintText);
        break;

    case GAME_OVER:
        window.draw(gameOverText);
        window.draw(finalScoreText);
        backToMenuFromGameOverButton->draw(window);

        escapeHintText.setString("(Esc to go back)");
        escapeHintText.setCharacterSize(14);
        escapeHintText.setFillColor(sf::Color(180, 180, 180));
        escapeHintText.setOrigin(0, escapeHintText.getLocalBounds().height / 2.f);
        escapeHintText.setPosition(10.f, WINDOW_HEIGHT - 20.f);
        window.draw(escapeHintText);
        break;
    }
    window.display();
}

// Main run loop
void Game::run() {
    while (window.isOpen()) {
        processEvents();
        update();
        render();
    }
}

int main() {
    Game game;
    game.run();
    return 0;
}
