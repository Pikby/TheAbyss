
//Creates a window using GLFW and turns on all the settings
GLFWwindow* createWindow(int width, int height);

void saveLevel(std::vector<Block>* level, const char* path);

std::vector <Block> loadLevel(const char* path);

void generateWhiteNoise(int width, int height, float** array);

void generateSmoothNoise(int width, int height, float** array, int octave);

void generateLand(int width, int height, float** array, int octCount);

float interpolate(float x0,float x1, float alpha);
