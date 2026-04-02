# Conway's Game Of Life

Personal implementation of the game

## Dependencies:
### Raylib:
#### [Get Raylib working on Arch Linux](https://github.com/raysan5/raylib/wiki/Working-on-GNU-Linux):

 - sudo pacman -S --needed raylib

#### To compile: 

 - gcc -o game-of-life game-of-life.c $(pkg-config --cflags --libs raylib)

