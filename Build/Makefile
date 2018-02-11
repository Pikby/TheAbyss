# the compiler: gcc for C program, define as g++ for C++
CC = g++
WCC = /media/WorkSpace/MXE/mxe/usr/bin/i686-w64-mingw32.static-g++
WPATH = /media/WorkSpace/MXE/mxe/usr/i686-w64-mingw32.static

WCC = /media/WorkSpace/MXE/mxe/usr/bin/i686-w64-mingw32.static-g++
CFLAGSLINUX  = -I/usr/local/include/freetype2 -I/usr/include/libpng12 \
					-lglfw3 -lGLEW -lGL -lm -ldl -lXinerama -lXrandr -lXi \
					-lXcursor -lX11 -lXxf86vm -lpthread -lSOIL -std=c++17 \
					-L/usr/local/lib -lfreetype -lstdc++fs -lboost_system -lboost_filesystem

CFLAGSWINDOWS =-I/usr/local/include/freetype2 -I/usr/include/libpng12  \
		-mwindows -lSOIL -lmingw32 -lGLEW \
		-lopengl32 -lm `/media/WorkSpace/MXE/mxe/usr/bin/i686-w64-mingw32.static-pkg-config --libs glfw3` \
		-lglu32 -lglew32 -lglew32s -std=c++17 -lpng -pthread -lz -lfreetype \
		`/media/WorkSpace/MXE/mxe/usr/bin/i686-w64-mingw32.static-pkg-config --libs freetype2` \
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

OBJECTLIST = main.o world.o openGL.o bsp.o camera.o gui.o mainchar.o FastNoise.o bspterraingen.o shaders.o inputhandling.o items.o

OFLAGS = g++ -c $(CFLAGSLINUX) -O3
# Main Engine
default: linux


ME = ../src/MainEngine
main.o: $(ME)/main.cpp
	 $(OFLAGS) $(ME)/main.cpp

world.o: $(ME)/world.cpp
	$(OFLAGS) $(ME)/world.cpp

openGL.o: $(ME)/openGL.cpp
	$(OFLAGS) $(ME)/openGL.cpp

bsp.o: $(ME)/bsp.cpp
	$(OFLAGS) $(ME)/bsp.cpp

# Character
CH = ../src/Character
camera.o: $(CH)/camera.cpp
	$(OFLAGS) $(CH)/camera.cpp

gui.o: $(CH)/gui.cpp
	$(OFLAGS) $(CH)/gui.cpp

mainchar.o: $(CH)/mainchar.cpp
	$(OFLAGS) $(CH)/mainchar.cpp

# Terrain Gen
TG = ../src/TerrainGeneration
FastNoise.o: $(TG)/FastNoise.cpp
	$(OFLAGS) $(TG)/FastNoise.cpp

bspterraingen.o: $(TG)/bspterraingen.cpp
	$(OFLAGS) $(TG)/bspterraingen.cpp

#Shaders

shaders.o: ../src/Shader/shaders.cpp
	$(OFLAGS) ../src/Shader/shaders.cpp

#IO
IO = ../src/InputHandling
inputhandling.o: $(IO)/inputhandling.cpp
	$(OFLAGS) $(IO)/inputhandling.cpp

#Inventory
IT = ../src/Inventory
items.o: $(IT)/items.cpp
	$(OFLAGS) $(IT)/items.cpp

linux: $(OBJECTLIST)
	$(CC) -o linux  $(OBJECTLIST) $(CFLAGSLINUX) -O3

windows:
	$(WCC) $(TARGET) $(CFLAGSWINDOWS) -o game.exe

clean:
	rm *.o
