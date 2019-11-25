So, this project doesn't really have a name yet but here is some documentation for consistency as there is alot to cover
Essentially, this project is a am attempt to recreate a minecraft like voxel engine from the ground up in C++.

## External Libraries Used:
* ### Graphics Rendering:
  * glfw3
  * GLEW
  * freetype2 for text rendering
  * stb_image for texture loading
* ### Math:
  * glm
* ### Others:
  * Boost for many qol things

## Compiling
Enter the build folder and use make linux for a linux build, and make server for the server linux build

For compiling to Linux, make sure that all the mentioned dependencies are installed
For cross compiling from Linux to Windows, MXE is used
There currently is a setup for compiling on windows, although it should not be that difficult

