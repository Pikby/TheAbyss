#pragma once
#include <experimental/filesystem>
#include <string>
#include "SOIL.h"
#include <iostream>
#include <math.h>
#include <vector>
#include <map>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <thread>
#include <queue>

// GLEW
// #define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext.hpp>

//Add the shader configs
#include "FastNoise.h"
#include "shaders.h"
#include "camera.h"
#include "bsp.h"

#include "openGL.h"
#include "mainchar.h"
#include "text.h"
