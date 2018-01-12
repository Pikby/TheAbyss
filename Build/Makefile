# the compiler: gcc for C program, define as g++ for C++
CC = g++
WCC = /media/kibby/WorkSpace/MXE/mxe/usr/bin/i686-w64-mingw32.static-g++
WPATH = /media/kibby/WorkSpace/MXE/mxe/usr/i686-w64-mingw32.static

WCC = /media/kibby/WorkSpace/MXE/mxe/usr/bin/i686-w64-mingw32.static-g++
CFLAGSLINUX  = -I/usr/local/include/freetype2 -I/usr/include/libpng12 \
					-lglfw3 -lGLEW -lGL -lm -ldl -lXinerama -lXrandr -lXi \
					-lXcursor -lX11 -lXxf86vm -lpthread -lSOIL -std=c++17 \
					-L/usr/local/lib -lfreetype -lstdc++fs -lboost_system -lboost_filesystem

CFLAGSWINDOWS =-I/usr/local/include/freetype2 -I/usr/include/libpng12  \
		-mwindows -lSOIL -lmingw32 -lGLEW \
		-lopengl32 -lm `/media/kibby/WorkSpace/MXE/mxe/usr/bin/i686-w64-mingw32.static-pkg-config --libs glfw3` \
		-lglu32 -lglew32 -lglew32s -std=c++17 -lpng -pthread -lz -lfreetype \
		`/media/kibby/WorkSpace/MXE/mxe/usr/bin/i686-w64-mingw32.static-pkg-config --libs freetype2` \
		-lboost_filesystem-mt -lboost_system-mt \
		-lboost_serialization-mt \
		-lboost_thread_win32-mt \
		-lboost_system-mt \
		-lboost_chrono-mt \
		-lboost_context-mt


FPATH = ../src/
TARGET = MainEngine/world.cpp MainEngine/main.cpp MainEngine/openGL.cpp MainEngine/bsp.cpp \
				 Character/camera.cpp Character/gui.cpp TerrainGeneration/FastNoise.cpp\
				 Character/mainchar.cpp TerrainGeneration/bspterraingen.cpp Shader/shaders.cpp \
				 InputHandling/inputhandling.cpp Inventory/items.cpp
NAME = -o game
linux:
	cd ../src &&	$(CC) $(NAME) $(TARGET) $(CFLAGSLINUX)
	mv ../src/game ../Build

windows: $(TARGET)
	$(WCC) $(NAME) $(TARGET) $(CFLAGSWINDOWS) -o game.exe
