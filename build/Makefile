# the compiler: gcc for C program, define as g++ for C++
CC = g++
WCC = /media/kibby/WorkSpace/MXE/mxe/usr/bin/i686-w64-mingw32.static.posix-g++
WPATH = /media/kibby/WorkSpace/MXE/mxe/usr/i686-w64-mingw32.static.posix-g++

WCC = /media/kibby/WorkSpace/MXE/mxe/usr/bin/i686-w64-mingw32.static.posix-g++
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
TARGET = MainEngine/world.cpp MainEngine/main.cpp MainEngine/threads.cpp MainEngine/bsp.cpp MainEngine/entities.cpp \
				 Character/gui.cpp TerrainGeneration/FastNoise.cpp\
				 Character/mainchar.cpp TerrainGeneration/bspterraingen.cpp  \
				 InputHandling/inputhandling.cpp Inventory/items.cpp \


COBJECTLIST = ctextureloading.o cmain.o cworld.o cobjects.o cthreads.o cbsp.o cgui.o cmainchar.o cFastNoise.o cbspterraingen.o cinputhandling.o citems.o
SOBJECTLIST = smain.o sworld.o sbsp.o sbspterraingen.o sFastNoise.o sitems.o sclients.o

OFLAGS = g++ -c $(CFLAGSLINUX) -O3
# Main Engine
default: linux

# Server
smain.o: ../serversrc/MainEngine/main.cpp
	$(OFLAGS) ../serversrc/MainEngine/main.cpp -o smain.o

sworld.o: ../serversrc/MainEngine/world.cpp
	$(OFLAGS) ../serversrc/MainEngine/world.cpp -o sworld.o

sbsp.o: ../serversrc/MainEngine/bsp.cpp
	$(OFLAGS) ../serversrc/MainEngine/bsp.cpp -o sbsp.o

sbspterraingen.o: ../serversrc/TerrainGeneration/bspterraingen.cpp
	$(OFLAGS) ../serversrc/TerrainGeneration/bspterraingen.cpp -o sbspterraingen.o

sFastNoise.o: ../serversrc/TerrainGeneration/FastNoise.cpp
	$(OFLAGS) ../serversrc/TerrainGeneration/FastNoise.cpp -o sFastNoise.o

sitems.o: ../serversrc/Inventory/items.cpp
	$(OFLAGS) ../serversrc/Inventory/items.cpp -o sitems.o

sclients.o: ../serversrc/MainEngine/clients.cpp
	$(OFLAGS) ../serversrc/MainEngine/clients.cpp -o sclients.o
# Client

ME = ../src/MainEngine
cmain.o: $(ME)/main.cpp
	 $(OFLAGS) $(ME)/main.cpp -o cmain.o

cworld.o: $(ME)/world.cpp
	$(OFLAGS) $(ME)/world.cpp -o cworld.o

cthreads.o: $(ME)/threads.cpp
	$(OFLAGS) $(ME)/threads.cpp -o cthreads.o

cbsp.o: $(ME)/bsp.cpp
	$(OFLAGS) $(ME)/bsp.cpp -o cbsp.o

# Character
CH = ../src/Character

cgui.o: $(CH)/gui.cpp
	$(OFLAGS) $(CH)/gui.cpp -o cgui.o

cmainchar.o: $(CH)/mainchar.cpp
	$(OFLAGS) $(CH)/mainchar.cpp -o cmainchar.o

# Terrain Gen
TG = ../src/TerrainGeneration
cFastNoise.o: $(TG)/FastNoise.cpp
	$(OFLAGS) $(TG)/FastNoise.cpp -o cFastNoise.o

cbspterraingen.o: $(TG)/bspterraingen.cpp
	$(OFLAGS) $(TG)/bspterraingen.cpp -o cbspterraingen.o


#IO
IO = ../src/InputHandling
cinputhandling.o: $(IO)/inputhandling.cpp
	$(OFLAGS) $(IO)/inputhandling.cpp -o cinputhandling.o

#Inventory
IT = ../src/Inventory
citems.o: $(IT)/items.cpp
	$(OFLAGS) $(IT)/items.cpp -o citems.o

cobjects.o: $(IT)/objects.cpp
	$(OFLAGS) $(IT)/objects.cpp -o cobjects.o

#TextureObject

ctextureloading.o: ../src/TextureLoading/textureLoading.cpp
	$(OFLAGS) ../src/TextureLoading/textureLoading.cpp -o ctextureloading.o


linux: $(COBJECTLIST)
	$(CC) -o linux  $(COBJECTLIST) $(CFLAGSLINUX) -O3

windows:
	cd ../src && $(WCC) -o windows.exe $(TARGET) $(CFLAGSWINDOWS)
	mv ../src/windows.exe ../build

server: $(SOBJECTLIST)
	$(CC) -o server $(SOBJECTLIST) $(CFLAGSLINUX) -O3


clean:
	rm *.o
