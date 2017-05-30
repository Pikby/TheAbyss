
#include "SOIL.h"
#include <iostream>
#include <math.h>

// GLEW
// #define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>



#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


//Add the shader configs
#include "shaders.h"

// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;
glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f,  3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);



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
    glViewport(0, 0, WIDTH, HEIGHT);

    glEnable(GL_DEPTH_TEST);

    //Making a new Shader using the configure fragment and vertex shader
    Shader newShader("shaders/shader.vs","shaders/shader.fs");



    //Vertices are in the format {x1,y1,z1,x2,y2,z2,....,}
    //Can have more the 3 vertices
    //In this example each point has an associated color to it

    GLfloat vertices[] =
    {
        // Positions          // Colors           // Texture Coords
         0.5f,  0.5f, 0.25f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // Top Right
         0.5f, -0.0f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // Bottom Right
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // Bottom Left
         0.0f, -0.5f, 1.0f,   1.0f, 1.0f, 1.0f,   0.0f, 1.0f,
    };

    GLuint indices[] =
    {
      0,1,2,
      1,2,3
    };

    //Now creating the buffer objects
    //VBO = vertex buffer object || VAO = Vertex Array Object || EBO = Element Buffer Object
    GLuint VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &EBO);
    //Generates the buffer id aka VAO
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

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(indices),indices, GL_STATIC_DRAW);

    //Vertices
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    //Colors
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    //Textures
    glVertexAttribPointer(2, 2, GL_FLOAT,GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    //Now for an attempt at the texture


    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);


    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// Set texture wrapping to GL_REPEAT (usually basic wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int imgWidth, imgHeight;
    unsigned char* image = SOIL_load_image("textures/tilesf1.jpg", &imgWidth, &imgHeight, 0, SOIL_LOAD_RGB);
    /*
    1. Specify the type of texture to be applied, can be between 1-3D
    2. Specify the mimmap level if we want do to that manually, keep it 0 for automatic
    3. The type of format of the texture etc. RBG or YRGB or smth idfk
    4. Sets the width
    5. Sets the Height
    6. IDK it says legacy stuff that they havent removed so keep it 0
    7. format of the image
    8. Data type of the image
    9.
    */
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imgWidth, imgHeight,0,GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);

    SOIL_free_image_data(image);
    glBindTexture(GL_TEXTURE_2D, 0);


    // Main program loop
    // Runs until the window is told to close
    float x = 0;
    while (!glfwWindowShouldClose(window))
    {
        // Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
        glfwPollEvents();

        // Render
        // Clear the colorbuffer
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);	// Activate the texture unit first before binding texture
        glBindTexture(GL_TEXTURE_2D, texture);
        //Use the newly made shader
        newShader.Use();

        glm::mat4 model;
        glm::mat4 view;
        glm::mat4 projection;





        model = glm::rotate(model, (GLfloat)glfwGetTime() * 2.0f, glm::vec3(0.5f, 1.0f, 0.0f));
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        // Note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.
        projection = glm::perspective(45.0f, (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);
        // Get their uniform location
        GLint modelLoc = glGetUniformLocation(newShader.Program, "model");
        GLint viewLoc = glGetUniformLocation(newShader.Program, "view");
        GLint projLoc = glGetUniformLocation(newShader.Program, "projection");
        // Pass them to the shaders
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

        // Create transformations
        glm::mat4 transform;
        transform = glm::translate(transform, glm::vec3(0.5f, -0.5f, 0.0f));
        transform = glm::rotate(transform,(GLfloat)glfwGetTime()* 2.0f, glm::vec3(0.0f, 0.0f, 1.0f));

        // Get matrix's uniform location and set matrix
        GLint transformLoc = glGetUniformLocation(newShader.Program, "transform");
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

        //Telling the program to register the vertices as a triangle and draw a triangle using our shader program
        glBindTexture(GL_TEXTURE_2D, texture);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT,0);
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
        GLfloat cameraSpeed = 0.05f;
    if(key == GLFW_KEY_W)
        cameraPos += cameraSpeed * cameraFront;
    if(key == GLFW_KEY_S)
        cameraPos -= cameraSpeed * cameraFront;
    if(key == GLFW_KEY_A)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if(key == GLFW_KEY_D)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}
