# The Abyss

The abyss is a voxel style sandbox video game in development in C++ using OpenGL. It's being developed completely from the ground up in an attempt to bring a heavily optimized voxel engine with more modern capabilities then that of the traditional "Minecraft" engine. Examples include real time cascaded shadow mapping, as well as infinite terrain generation in all 6 cardinal directions. It is currently in development in a Linux environment, although windows cross compiling is also implemented.

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
Make sure all external libraries are setup or installed within the enviroment. For linux client compiling enter the Build folder and build "linux" using the makefile. For linux server compiling enter the Build foler and build "server" using the makefile. A windows crosscompile on linux is also available through [MXE](https://github.com/mxe/mxe), although the makefile must be told where the MXE compiler is located.


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
