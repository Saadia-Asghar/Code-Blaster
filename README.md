# Code Blaster

**Code Blaster** is a fun and educational SFML-based quiz shooting game focused on teaching C++ programming concepts through multiple-choice questions. Players answer questions to shoot down challenges and progress through lessons while managing lives and scores.

---

## Game Overview

- Multiple levels themed as C++ lessons covering variables, functions, OOP, loops, arrays, pointers, and more.
- Shoot correct answers to earn points and advance.
- Incorrect answers reduce the player's lives.
- Complete a level to unlock the next one.
- Animated effects and sounds enhance the user experience.

---

## Features

- Smooth SFML graphics and animations.
- Engaging shooting mechanic combined with quiz gameplay.
- Multiple-choice questions with visual feedback.
- Level progression and unlock system.
- Background music and sound effects.
- Responsive UI with buttons and mult-line text support.
- Explosion animation for incorrect answers.

---

## Project Structure
CodeBlaster/ │ 
├── src/ 
│ └── main.cpp # Main game source file │ 
   ├── resources/ │ 
       ├── sansation.ttf # Font used in the game │
       ├── explosion-42132.wav # Sound for correct answers │
       ├── buzzer-or-wrong-answer-20582.wav # Sound for incorrect answers │ 
       ├── the-world-of-8-bit-games-301273.wav # Background music │
       ├── explosion2.png # Explosion animation spritesheet │ 
       ├── Blue Green Pixel Retro Quiz Trivia Game Presentation.png # Background image │ 
       ├── spaceMeteors_002.png # Home screen image │ 
       ├── spaceRockets_001.png # Shooter texture │ └── spaceMissiles_009.png # Bullet texture │ └── README.md # This documentation file

Run
Copy code

---

## Requirements

- C++ compiler supporting C++11 or later
- [SFML 2.x library](https://www.sfml-dev.org/download.php) installed and linked
- Compatible development environment (e.g., Visual Studio, Code::Blocks, or g++ command line)
- Assets placed correctly in the `resources/` folder or paths updated in the code

---

## How to Build and Run

1. **Clone or download this repository**:
   ```bash
   git clone https://github.com/Saadia-Asghar/Code-Blaster
   cd CodeBlaster
