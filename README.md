<div align="center">

# game-of-life

**An implementation of Conway's Game of Life**

![C](https://img.shields.io/badge/C-C99-00599C?logo=c&logoColor=white)
![raylib](https://img.shields.io/badge/raylib-6.2-black?logo=raylib&logoColor=white)
![raygui](https://img.shields.io/badge/raygui-4.0-black?logo=raylib&logoColor=white)
![Make](https://img.shields.io/badge/build-Make-A42E2B?logo=gnu&logoColor=white)
![Web](https://img.shields.io/badge/web-WebAssembly-654FF0?logo=webassembly&logoColor=white)
![Platform](https://img.shields.io/badge/platform-Linux%20%7C%20Web-lightgrey)
![Status](https://img.shields.io/badge/status-in%20development-orange)

</div>

- The grid evolves following the classic rules: a live cell with 2 or 3 live neighbours survives, a dead cell with exactly 3 live neighbours comes to life, and every other cell dies or stays dead.

- Each step is computed in two passes (first every change is determined, then they are all applied) so that all cells update at the same time.

- Initial patterns are drawn by hand in edit mode, clicking any cell to switch it between alive and dead.

- The steps per second and the number of horizontal tiles can be edited from the settings screen.

- Tiles are always square, so the number of vertical tiles is calculated from the window size.

- The window can be resized freely and the grid adapts to it.


## Controls

Everything can be done with the on-screen buttons or with the following shortcuts:
| Key | Action |
|---|---|
| Space | Start, pause or resume the simulation |
| R | Restart the simulation with initial values |
| S | Open settings |
| B | Go back to the menu from settings |
| E | Enter or exit edit mode |
| Left click | Switch the clicked cell between alive and dead (in edit mode) |
| Enter | Apply the value typed in the selected text field in settings |


## Build

The project uses Make to build and is currently configured with the following options:
| Command | Use case |
|---|---|
| make run | To compile and run for desktop. Needs [Raylib](https://www.raylib.com/) and [raygui](https://github.com/raysan5/raygui) installed. Right now it's only configured for Linux although I might expand to other platforms in the future |
| make debug | To compile with debugger friendly flags (no optimizations) |
| make web | To compile to WASM. Needs the [Emscripten](https://emscripten.org/) compiler, an installation of [Raylib](https://www.raylib.com/) built for WASM and the [raygui](https://github.com/raysan5/raygui) header (paths must be provided in the .env file, see .example.env) |
| make serve | To serve the WASM build in a local server using python |


## Status

In development. Live [here](https://bentor.dev/simulations).


## License

Copyright (c) 2026 bentor. All rights reserved.

No license is granted to use, copy, modify or distribute this code.
