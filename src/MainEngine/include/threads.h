//Creates a window using GLFW and turns on all the settings




class ThreadHandler
{
private:
  static GLFWwindow* window;
  static bool threadsOn;
public:
  static void dispatchThreads();
  static void endThreads();
  static GLFWwindow* createWindow(int width, int height);
  static void enableCursor();
  static void disableCursor();
  static void logic();
  static void draw();
  static void render();
  static void del();
  static void mainBuild();
  static void receive();
  static void send();
  static void build(char i);
  static void initWorld(const std::string& ip,const std::string& userName);
  static void closeGame();
};
