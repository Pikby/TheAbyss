# the compiler: gcc for C program, define as g++ for C++
CC = g++

# compiler flags:
#  -g    adds debugging information to the executable file
#  -Wall turns on most, but not all, compiler warnings
CFLAGS  = -lglfw3 -lGLEW -lGL -lm -ldl -lXinerama -lXrandr -lXi -lXcursor -lX11 -lXxf86vm -lpthread -lSOIL -std=c++11

# the build target executable:
TARGET = ../src/main.cpp ../src/shaders.cpp ../src/openGL.cpp ../src/block.cpp

all: $(TARGET)
	$(CC) $(TARGET) $(CFLAGS)
