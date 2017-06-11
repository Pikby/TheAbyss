
//Creates a window using GLFW and turns on all the settings
GLFWwindow* createWindow(int width, int height);

void saveLevel(std::vector<Block>* level, const char* path);

std::vector <Block> loadLevel(const char* path);
