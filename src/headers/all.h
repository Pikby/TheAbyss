#pragma once


#ifdef _WIN32
  /* See http://stackoverflow.com/questions/12765743/getaddrinfo-on-win32 */
  #ifndef _WIN32_WINNT
    #define _WIN32_WINNT 0x0501  /* Windows XP. */
  #endif
  #include <winsock2.h>
  #include <ws2tcpip.h>
#else
  /* Assume that any non-Windows platform uses POSIX-style sockets instead. */
  #include <sys/socket.h>
  #include <arpa/inet.h>
  #include <netdb.h>  /* Needed for getaddrinfo() and freeaddrinfo() */
  #include <unistd.h> /* Needed for close() */
#endif
//enum renderType{ OPAQUE,TRANSLUCENT,TRANSPARENT};

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
#include <atomic>


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

#include "entity.h"

#include "bsp.h"
#include "openGL.h"
#include "gui.h"
#include "mainchar.h"
#include "inputhandling.h"
