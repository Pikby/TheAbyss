#pragma once

#include <boost/filesystem.hpp>
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
#include <pthread.h>



#define PI 3.14159265
// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext.hpp>

#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <thread>
#include <fstream>
#include <bitset>
#include <mutex>

#define PORT 3030
#define ADDRESS "127.0.0.1"

#include "3dmap.h"

typedef unsigned char uchar;
//Add the shader configs

#include "FastNoise.h"
#include "shaders.h"
#include "camera.h"
#include "bsp.h"
#include "openGL.h"
#include "gui.h"
#include "mainchar.h"
#include "inputhandling.h"
