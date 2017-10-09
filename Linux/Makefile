# the compiler: gcc for C program, define as g++ for C++
CC = g++
WCC = x86_64-w64-mingw32-g++

CFLAGSLINUX  = -I/usr/local/include/freetype2 -I/usr/include/libpng12 \
					-lglfw3 -lGLEW -lGL -lm -ldl -lXinerama -lXrandr -lXi \
					-lXcursor -lX11 -lXxf86vm -lpthread -lSOIL -std=c++11 \
					-L/usr/local/lib -lfreetype -lstdc++fs

CFLAGSWINDOWS = -I/usr/local/include/freetype2\
-lglfw3 -lGL -lGLEW -lm -ldl -lXinerama -lXrandr -lXi \
-lXcursor -lX11 -lXxf86vm -lpthread -lSOIL -std=c++11 \

TARGET = ../src/world.cpp ../src/main.cpp ../src/shaders.cpp ../src/openGL.cpp \
				 ../src/camera.cpp   ../src/gui.cpp ../src/FastNoise.cpp\
				 ../src/bsp.cpp ../src/mainchar.cpp ../src/bspterraingen.cpp
#TARGET = ../src/test.cpp ../src/shaders.cpp
NAME = -o game
linux: $(TARGET)
	$(CC) $(NAME) $(TARGET) $(CFLAGSLINUX)

windows: $(TARGET)
	$(WCC) $(NAME) $(TARGET) $(CFLAGSWINDOWS) -o One
