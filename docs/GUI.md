

The GUI library found [Here](../src/Character/include/gui.h) is a custom made GUI library written in c++.

The gui library has access to a list of functionalities:

* draw Image
* draw Quad with rendering styles found [here](../src/Shaders/GUIShaders/guiShader2D.fs)
* draw Triangle
* glfwCallbacks
* and text rendering found in [Here](../src/Character/Widgets/textrendering.cpp)

The text rendering is an implementation of Valve's paper on [Texture Rendering](https://steamcdn-a.akamaihd.net/apps/valve/2007/SIGGRAPH2007_AlphaTestedMagnification.pdf). Adapted for the the basic ascii characters. The text rendering engine implements a batch rendering function, to allow for a lot of characters on screen without a performance loss.
Unicode is not supported at this time

The GUI library is then implemented inside of [Widgets](../src/Character/Widgets/widgets.h), a virtual class with flexible subclasses.

The current widgets are:
* [Quad](../src/Character/Widgets)
* [Edit Box](../src/Character/Widgets/label.cpp): a clickable label with editable text
* [Button](../src/Character/Widgets/button.cpp): a clickable button which triggers a custom function
* [ChatBox](../src/Character/Widgets/chatbox.cpp): a custom chatbox used for storing and displaying chat messages.
* [Image Grid](../src/Character/Widgets/grid.cpp): a widget used to display a grid of images, interact-able.  
* [Widget List](../src/Character/Widgets/grid.cpp): a widget containing a bunch of subwidgets
* [In Game](../src/Character/Widgets/ingame.cpp): a widget used for rerouting mouse and keyboard commands back to the game.

The widgets can then be used to create full [Menus](../src/Character/include/menus.h) which can be swapped in and out during usage.

Finally, all inputs are fed to the GUI library, which are then fed to the relative menu, which then feeds them to the relative widget for user interaction.
