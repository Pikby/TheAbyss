# the compiler: gcc for C program, define as g++ for C++
CC = g++
WCC = x86_64-w64-mingw32-g++

CFLAGSLINUX  = -I/usr/local/include/freetype2 -I/usr/include/libpng12 \
					-lglfw3 -lGLEW -lGL -lm -ldl -lXinerama -lXrandr -lXi \
					-lXcursor -lX11 -lXxf86vm -lpthread -lSOIL -std=c++11 \
					-L/usr/local/lib -lfreetype

CFLAGSWINDOWS = -I/usr/local/include/freetype2\
-lglfw3 -lGL -lGLEW -lm -ldl -lXinerama -lXrandr -lXi \
-lXcursor -lX11 -lXxf86vm -lpthread -lSOIL -std=c++11 \

TARGET = ../src/world.cpp ../src/main.cpp ../src/shaders.cpp ../src/openGL.cpp \
				 ../src/camera.cpp   ../src/text.cpp \
				 ../src/bsp.cpp ../src/mainchar.cpp
#TARGET = ../src/test.cpp ../src/shaders.cpp
linux: $(TARGET)
	$(CC)  $(TARGET) $(CFLAGSLINUX)

windows: $(TARGET)
	$(WCC) $(TARGET) $(CFLAGSWINDOWS) 
