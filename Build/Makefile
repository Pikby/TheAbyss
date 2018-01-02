# the compiler: gcc for C program, define as g++ for C++
CC = g++
WCC = /media/kibby/WorkSpace/MXE/mxe/usr/bin/i686-w64-mingw32.static-g++
WPATH = /media/kibby/WorkSpace/MXE/mxe/usr/i686-w64-mingw32.static

WCC = /media/kibby/WorkSpace/MXE/mxe/usr/bin/i686-w64-mingw32.static-g++
CFLAGSLINUX  = -I/usr/local/include/freetype2 -I/usr/include/libpng12 \
					-lglfw3 -lGLEW -lGL -lm -ldl -lXinerama -lXrandr -lXi \
					-lXcursor -lX11 -lXxf86vm -lpthread -lSOIL -std=c++17 \
					-L/usr/local/lib -lfreetype -lstdc++fs

CFLAGSWINDOWS =-I/usr/local/include/freetype2 -I/usr/include/libpng12  \
		-mwindows -lSOIL -lmingw32 -lGLEW \
		-lopengl32 -lm `/media/kibby/WorkSpace/MXE/mxe/usr/bin/i686-w64-mingw32.static-pkg-config --libs glfw3` \
		-lglu32 -lglew32 -lglew32s -std=c++17 -lpng -pthread -lz -lfreetype \
		`/media/kibby/WorkSpace/MXE/mxe/usr/bin/i686-w64-mingw32.static-pkg-config --libs freetype2`



TARGET = ../src/world.cpp ../src/main.cpp ../src/shaders.cpp ../src/openGL.cpp \
				 ../src/camera.cpp   ../src/gui.cpp ../src/FastNoise.cpp\
				 ../src/bsp.cpp ../src/mainchar.cpp ../src/bspterraingen.cpp
NAME = -o game
linux: $(TARGET)
	$(CC) $(NAME) $(TARGET) $(CFLAGSLINUX)

windows: $(TARGET)
	$(WCC) $(NAME) $(TARGET) $(CFLAGSWINDOWS) -o game.exe
