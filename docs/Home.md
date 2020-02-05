# The Abyss
Welcome to the Docs of the abyss
* Go [Here](GUI.md) for information on the GUI implementation.
* Go [Here](MainEngine.md) for the overall engine implementation.
* Go [Here](Meshing.md) for the meshing implementation.
* Go [Here](Threads.md) for thread overview.


Finally a todo list can be found [Here](TODO.md)

The makefile for this project can be found [Here](../build/Makefile)
## External Libraries Used:
* ### Graphics Rendering:
  * glfw3
  * GLEW
  * freetype2 for text rendering
  * stb_image for texture loading
  * byBullet for physics
* ### Math:
  * glm
* ### Others:
  * Boost for many qol things

## Compiling
Enter the build folder and use make linux for a linux build, and make server for the server linux build

For compiling to Linux, make sure that all the mentioned dependencies are installed
~~For cross compiling from Linux to Windows, MXE is used~~ Broken ATM
