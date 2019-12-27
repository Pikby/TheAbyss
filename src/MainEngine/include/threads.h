//Creates a window using GLFW and turns on all the settings

#include <thread>


class ThreadHandler
{
private:
  static GLFWwindow* window;
  static bool threadsOn;
  static std::thread renderThread,deleteThread,sendThread,receiveThread,logicThread;
public:
  static void dispatchThreads();
  static void endThreads();
  static GLFWwindow* createWindow(const GLFWvidmode* mode);
  static void enableCursor();
  static void disableCursor();
  static void logic();
  static void draw();
  static void render();
  static void del();
  static void mainBuild();
  static void receive();
  static void send();
  static void build();
  static void initWorld(const std::string& ip,const std::string& userName);
  static void closeGame();

};

#ifdef THREADIMPLEMENTATION
GLFWwindow* ThreadHandler::window;
bool ThreadHandler::threadsOn;

std::thread ThreadHandler::renderThread,ThreadHandler::deleteThread,ThreadHandler::sendThread,
            ThreadHandler::receiveThread,ThreadHandler::logicThread;


#endif
