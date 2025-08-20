# Multiplayer games
A classic snake game with embedded multiplayer support for up to 3 players, featuring persistent score storage in EEPROM <br>
A classic breakout game with embedded multiplayer support for up to 3 players, featuring persistent score storage in EEPROM.<br>
This is implemented in the InspireComputer:<br><br>
![Inspire Computer](https://github.com/user-attachments/assets/c666a969-0a7b-4e4c-9170-0d6622b96ce6)
## Features
- Multiplayer support for 3  players
- Persistent high score storage in EEPROM
- Simple name selection system

## Controls

### Name Selection (At Startup)
Create your names, and then choose any names you like
- *Button 1*: First Name
- *Button 4*: Second Name
- *Button 7*: Third Name
### Game Selection (At Startup)
Choose the game you like
- *Button 3*: Snake Game


### Score Management
- *Button 9*: View saved high scores (available after game over) for your selected name
- *Button 3*: Erase all saved scores (reset all high scores)
- *Button 7*: shows the highest score, and the corresponding name of a player 
=======

## Features

### Game Controls(Snake game)
- *2* - Move Up
- *4* - Move Left  
- *6* - Move Right
- *8* - Move Down
### Game Controls(Breakout Game)
- *4* - Move Left  
- *6* - Move Right
  
### Utility Buttons
- *5* - Reset game after Game Over
- *1* - Change game speed (for snake game only)

## Installation

### Prerequisites
- [MounRiver Studio](https://www.mounriver.com/download)
- WHCLink-E programmer
- Inspire RISC-V computer

### Setup(Snake Game)
1. Install MounRiver Studio
2. Open Snake_game.wvproj project
3. Click *Rebuild* to compile
4. Connect the Inspire computer via WHCLink-E
5. Download into the Inspire computer
6. Launch the game!

### Setup(Snake Game)
1. Install MounRiver Studio
2. unzip Brekout_game.zip in another folder, not in the same folder as snake game!, it is very important
3. open Breakout_game.wvproj project
4. Click *Rebuild* to compile
5. Connect the Inspire computer via WHCLink-E
6. Download into the Inspire computer
7. Launch the game!

## Gameplay(Snake)
- Guide the snake to eat food and grow
- Avoid walls and self-collision
- Higher speeds increase difficulty
- Previous scores are saved between sessions
## Gameplay(Breakout)
- Guide the paddle to hit the balls
- Avoid the ball from falling to the ground
- Previous scores are saved between sessions

## Contributors
- *A. Tong* - drivers and low level functions  

## Troubleshooting
- Ensure proper display connections
- Verify button input functionality
- Check programmer connection if flashing fails



