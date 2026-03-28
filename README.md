# Conway's Game Of Life

Personal implementation of the game


## Dependencies:
### Raylib:
#### List of commands to execute in orther to [get Raylib working on Arch Linux](https://github.com/raysan5/raylib/wiki/Working-on-GNU-Linux):
- sudo pacman -S alsa-lib mesa libx11 libxrandr libxi libxcursor libxinerama
- git clone --depth 1 https://github.com/raysan5/raylib.git raylib
- cd raylib/src/
- make PLATFORM=PLATFORM_DESKTOP # To make the static version.
- make PLATFORM=PLATFORM_DESKTOP RAYLIB_LIBTYPE=SHARED # To make the dynamic shared version.
- sudo make install # Static version.
- sudo make install RAYLIB_LIBTYPE=SHARED # Dynamic shared version.

#### Remove the library from the standard directories:
- cd raylib/src/
- sudo make uninstall
- sudo make uninstall RAYLIB_LIBTYPE=SHARED

## To compile and run: 
Important to remember the flag "-lraylib" when compiling.
 - clear && gcc -o game-of-life game-of-life.c -lraylib && ./game-of-life