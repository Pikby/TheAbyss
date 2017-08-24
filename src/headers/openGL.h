
//Creates a window using GLFW and turns on all the settings
GLFWwindow* createWindow(int width, int height);

void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

void* draw(void* );
void* render(void *);
void* del(void *);
void* mainBuild(void* );
void* build(void *i);
void initWorld(int numbBuildThreads);
