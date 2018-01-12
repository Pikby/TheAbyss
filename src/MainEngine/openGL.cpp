#include "../headers/all.h"

//Global maincharacter reference which encapsulates the camera
static World* newWorld;
static GLFWwindow* window;
static MainChar* mainCharacter;




GLFWwindow* createWindow(int width, int height)
{
  //Initial windows configs

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
  //Create the window
  window = glfwCreateWindow(width, height, "Prototype 1.000", nullptr, nullptr);

  return window;
}

void initWorld(int numbBuildThreads, int width,  int height)
{
  //glfwMakeContextCurrent(window);
  newWorld = new World(numbBuildThreads,width,height);
  mainCharacter = new MainChar(0,100,0,newWorld);
  initializeInputs(mainCharacter);

}

void closeGame()
{
  glfwSetWindowShouldClose(window, true);
}


void draw()
{

  float deltaTime = 0.0f;	// time between current frame and last frame
  float lastFrame = 0.0f;
  glfwMakeContextCurrent(window);
  long long totalFrame = 0;
  std::string fpsString;

  std::string path = "../src/Shader/shaders/";
  //Create the shaders
  Shader blockShader( path + "shaderBSP.vs",path + "shaderBSP.fs");
  Shader depthShader( path + "depthShader.vs",path + "depthShader.fs");
  Shader debugShader( path + "debugQuad.vs",path + "debugQuad.fs");

  blockShader.use();
  blockShader.setInt("curTexture", 0);
  blockShader.setInt("shadowMap", 1);
  /*
  unsigned int HDRbuffer;
  glBindTexture(GL_TEXTURE_2D, HDRbuffer);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F,curWorld->VIEW_WIDTH,curWorld->)
  */
  glm::vec3 lightPos;

  int shadowSize = 10024;
  int SHADOW_WIDTH = shadowSize;
  int SHADOW_HEIGHT = shadowSize;
  int VIEW_WIDTH = 1280;
  int VIEW_HEIGHT = 720;

  unsigned int depthMapFBO, depthMap;
  glGenFramebuffers(1,&depthMapFBO);

  glGenTextures(1, &depthMap);
  glBindTexture(GL_TEXTURE_2D, depthMap);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
               SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);


  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  int vertRenderDistance = 7;
  int horzRenderDistance = 7;
  int renderBuffer = 1;
  double sunAngle = 10;
  while(!glfwWindowShouldClose(window))
  {
    updateInputs();
    //std::cout << newWorld->drawnChunks << "\n";
    newWorld->drawnChunks = 0;

    totalFrame++;
	   // update the delta time each frame
	 float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    //Create a time for every second and displays the FPS

    glfwPollEvents();
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    mainCharacter->update();


    Camera camera = mainCharacter->mainCam;
    sunAngle += 0.001;

    //Calculate shadow matrices
    int distToSun = (vertRenderDistance+renderBuffer+1)*CHUNKSIZE;
    //Makes sure the light is always at the correct angle above the player
    lightPos.x = cos(sunAngle*PI/180)*distToSun+camera.position.x;
    lightPos.y = sin(sunAngle*PI/180)*distToSun+camera.position.y;
    lightPos.z = camera.position.z;
    float near = -1;
    float far = distToSun*2;
    glm::mat4 lightProjection,lightView, lightSpaceMatrix;
    float orthoSize = (horzRenderDistance+renderBuffer)*CHUNKSIZE;
    lightProjection = glm::ortho(-orthoSize, orthoSize, -orthoSize,orthoSize,near, far);
    lightView  = glm::lookAt(lightPos,
                                      camera.position,
                                      glm::vec3(0.0f,1.0f,0.0f));

    lightSpaceMatrix = lightProjection * lightView;
    depthShader.use();
    depthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

    //Draw the world for the shadow
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        glActiveTexture(GL_TEXTURE0);
        newWorld->drawWorld(lightSpaceMatrix,lightProjection,false);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, VIEW_WIDTH, VIEW_HEIGHT);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    //Setup the shader to draw the world
    blockShader.use();
    glm::mat4 camProjection = glm::perspective(glm::radians(45.0f),
                              (float)1920/ (float)1080, 0.1f,
                              (float)horzRenderDistance*CHUNKSIZE*4);
    glm::mat4 camView = camera.getViewMatrix();
    blockShader.setMat4("projection",camProjection);
    blockShader.setMat4("view", camView);
    blockShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
    blockShader.setVec3("lightColor",  1.0f, 1.0f, 1.0f);
    blockShader.setVec3("lightPos",  lightPos);
    blockShader.setVec3("viewPos", camera.position);
    blockShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);


    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, depthMap);

    glm::mat4 hsrProjection = glm::perspective(glm::radians(50.0f),
                              (float)1920/ (float)1080, 0.1f,
                              (float)horzRenderDistance*CHUNKSIZE*4);
    newWorld->drawWorld(camera.getHSRMatrix(), hsrProjection,true);


    mainCharacter->drawHud();
    //std::cout << glGetError() << "update loop\n";

    glfwSwapBuffers(window);
  }
  std::cout << "exiting draw thread \n";
}

void* render(void* )
{
  int renderLoop = 0;
  while(!glfwWindowShouldClose(window))
  {
    //std::cout << "finished render loop\n";
    newWorld->renderWorld(&mainCharacter->xpos,&mainCharacter->ypos,&mainCharacter->zpos);
    //std::cout << "Finished render loop" << renderLoop << "\n";
  }
  newWorld->saveWorld();
  std::cout << "exiting render thread \n";
  return NULL;
}

void* del(void* )
{
  while(!glfwWindowShouldClose(window))
  {
    //std::cout << "finished delete scan\n";
    newWorld->delScan(&mainCharacter->xpos,&mainCharacter->ypos,&mainCharacter->zpos);
  }
  std::cout << "exiting delete thread \n";
  return NULL;
}

void* build(void*i)
{
  int threadNumb = *(int*) i;
  delete (int*)i;
  while(!glfwWindowShouldClose(window))
  {
    newWorld->buildWorld(threadNumb);
  }
  std::cout << "exiting build thread #" << threadNumb << "\n";
  return NULL;
}
