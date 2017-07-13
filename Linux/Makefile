# the compiler: gcc for C program, define as g++ for C++
CC = g++


CFLAGS  = -I/usr/local/include/freetype2 -I/usr/include/libpng12 -lglfw3 -lGLEW -lGL -lm -ldl -lXinerama -lXrandr -lXi -lXcursor -lX11 -lXxf86vm -lpthread -lSOIL -std=c++11 -L/usr/local/lib -lfreetype

TARGET = ../src/main.cpp ../src/shaders.cpp ../src/openGL.cpp ../src/block.cpp ../src/camera.cpp  ../src/mainchar.cpp ../src/text.cpp ../src/bsp.cpp
#TARGET = ../src/test.cpp ../src/shaders.cpp
all: $(TARGET)
	$(CC)  $(TARGET) $(CFLAGS)
