# The Abyss

The Abyss is a voxel style sandbox engine built using C++ and OpenGL. It aims to be a heavily optimized voxel engine with more modern capabilities than that of the traditional "Minecraft" engine. Examples of additional capabilities include real time cascaded shadow mapping and infinite terrain generation in all 6 cardinal directions.

It is currently in development in a Linux environment. Windows cross compiling is also implemented.

### External Libraries Used:
* #### Graphics Rendering:
  * glfw3
  * GLEW
  * freetype2 for text rendering
  * stb_image for texture loading
* #### Math:
  * glm
* #### Others:
  * Boost for many qol things

## Compiling
- Make sure all external libraries mentioned above are installed and setup within the environment.
- For Linux client compiling, enter the Build folder and build "linux" using the makefile. 
- For Linux server compiling enter the Build folder and build "server" using the makefile.
- A Windows cross compile on Linux is also available through [MXE](https://github.com/mxe/mxe). The makefile must be told where the MXE compiler is located.


## Implemented:
* Cascaded Shadow mapping
* Multiple light sources
* Multiple dynamic point shadows
* Multiplayer using TCP sockets
* Translucent objects

## Todo List:
* Engine Optimizations/Stability
* Inventory system
* GUI
* Network stability
* Gameplay assets
