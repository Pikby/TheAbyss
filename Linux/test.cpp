

#include <iostream>
#include <math.h>

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

//Add the shader configs
#include "shaders.h"

// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;


int main()
{

    glfwInit();

    // Set all the required options for GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // Creates the main window
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "HEYGUYSITSATRIANGLE", nullptr, nullptr);
    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    //Focus on the newly made window
    glfwMakeContextCurrent(window);

    // Set the required callback functions
    glfwSetKeyCallback(window, key_callback);

    // Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
    glewExperimental = GL_TRUE;
    // Initialize GLEW to setup the OpenGL Function pointers
    if (glewInit() != GLEW_OK)
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    // Define the viewport dimensions
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);



    /*
    //Now onto compiling the shaders
    //First the vertexShader
    GLuint vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);

    //Attach the custom made vertexShaderSource onto the the vertexShader
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    //Compile the shader
    glCompileShader(vertexShader);

    //Second, the fragmentShader, its the same process as the vertexShader
    GLuint fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader,1 , &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    //Finally, the shader program which lins the multiple shaders we have
    GLuint shaderProgram;
    shaderProgram = glCreateProgram();

    //Attach the shaders
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    //Link the shaderProgram
    glLinkProgram(shaderProgram);
    //Clean up the shaders since they will no longer be used
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    //Shaders are now done, working on objects
    */
    Shader newShader("shader.vs","shader.fs");



    //Vertices are in the format {x1,y1,z1,x2,y2,z2,....,}
    //Can have more the 3 vertices
    GLfloat vertices[] = {
         // Positions         // Colors
         0.5f, -0.5f, 0.0f,   1.0f, 0.0f, 0.0f,  // Bottom Right
        -0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,  // Bottom Left
         0.0f,  0.5f, 0.0f,   0.0f, 0.0f, 1.0f   // Top
     };


    //Now creating the buffer objects
    //VBO = vertex buffer object || VAO = Vertex Array Object
    GLuint VBO, VAO;
    glGenVertexArrays(1, &VAO);
    //Generates the buffer id aka VBO
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    //Binds the array to the buffer

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    /*
    Copies the data into the currently bound buffer
    1.The Buffer
    2.Sizeof the the data to copy
    3.The data
    4. ow the data will be handled
    GL_STATIC_DRAW for static objects
    GL_DYNAMIC_DRAW for data that changes alot
    GL_STREAM_DRAW for data that changes everysingle frame
    */
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices),vertices, GL_STATIC_DRAW);
    /*
    Now to tell the compiler how to interpret the vertexs
    1.Pass in the intial location such as in the shaders
    2.Size of the vertex aka how many vertices are being sent in
    3.Type of vertex data aka GLfloat
    4.Boolean value for whether or not data should be normalized
    5.The size of one set of vertices aka sizeof(vertexType)*numberofdimensions per vertice
    6.??????
    */

    //Vertices
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    //Colors
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Main program loop
    // Runs until the window is told to close
    while (!glfwWindowShouldClose(window))
    {
        // Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
        glfwPollEvents();

        // Render
        // Clear the colorbuffer
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        newShader.Use();
        //Telling the program to register the vertices as a triangle and draw a triangle using our shader program
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);



        // Swap the screen buffers
        glfwSwapBuffers(window);
    }

    // Terminate GLFW, clearing any resources allocated by GLFW.
    glfwTerminate();
    return 0;
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    std::cout << key << std::endl;
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}
