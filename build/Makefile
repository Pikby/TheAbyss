# the compiler: gcc for C program, define as g++ for C++
default: linux
CC = g++
WCC = /mnt/WorkSpace/MXE/mxe/usr/bin/i686-w64-mingw32.static.posix-g++
WPATH = /mnt/WorkSpace/MXE/mxe/usr/i686-w64-mingw32.static.posix-g++

WCC = /mnt/WorkSpace/MXE/mxe/usr/bin/i686-w64-mingw32.static.posix-g++
CFLAGSLINUX  = -I/usr/local/include/freetype2 -I/usr/include/libpng12 \
					-lglfw3 -lGLEW -lGL -lm -ldl -lXinerama -lXrandr -lXi \
					-lXcursor -lX11 -lXxf86vm -lpthread -std=c++17 \
					-L/usr/local/lib -lfreetype -lstdc++fs -lboost_system -lboost_filesystem

CFLAGSWINDOWS =-I/usr/local/include/freetype2 -I/usr/include/libpng12  \
		-mwindows -lmingw32 -lGLEW \
		-lopengl32 -lm `/mnt/WorkSpace/MXE/mxe/usr/bin/i686-w64-mingw32.static-pkg-config --libs glfw3` \
		-lglu32 -lglew32 -lglew32s -std=c++17 -lpng -pthread -lz -lfreetype \
		`/mnt/WorkSpace/MXE/mxe/usr/bin/i686-w64-mingw32.static-pkg-config --libs freetype2` \
		-lboost_filesystem-mt -lboost_system-mt \
		-lboost_serialization-mt \
		-lboost_thread_win32-mt \
		-lboost_system-mt \
		-lboost_chrono-mt \
		-lboost_context-mt


FPATH = ../src/
TARGET = MainEngine/world.cpp MainEngine/main.cpp MainEngine/threads.cpp MainEngine/bsp.cpp MainEngine/drawer.cpp MainEngine/messenger.cpp \
				 Objects/items.cpp Objects/objects.cpp \
				 Character/gui.cpp TerrainGeneration/FastNoise.cpp \
				 Character/mainchar.cpp TerrainGeneration/bspterraingen.cpp  \
				 InputHandling/inputhandling.cpp \
				 Settings/settings.cpp \
				 TextureLoading/textureLoading.cpp



COBJECTLIST = csettings.o cdrawer.o ctextureloading.o cmain.o cworld.o cmessenger.o cobjects.o cthreads.o \
							cbsp.o cgui.o cmainchar.o cinputhandling.o \
							citems.o
SOBJECTLIST = smain.o sworld.o sbsp.o sbspterraingen.o sFastNoise.o sitems.o sclients.o scommands.o sserver.o $(FASTNOISEOBJECTS)


FASTNOISEOBJECTS = FastNoiseSIMD.o FastNoiseSIMD_avx2.o FastNoiseSIMD_internal.o FastNoiseSIMD_sse2.o FastNoiseSIMD_sse41.o
NP =  ../serversrc/TerrainGeneration
NFLAGS = g++ -std=c++17 -march=core-avx2 -c

FastNoiseSIMD.o: $(NP)/FastNoiseSIMD.cpp
	$(NFLAGS) $(NP)/FastNoiseSIMD.cpp -o FastNoiseSIMD.o

FastNoiseSIMD_avx2.o: $(NP)/FastNoiseSIMD_avx2.cpp
	$(NFLAGS) $(NP)/FastNoiseSIMD_avx2.cpp -o FastNoiseSIMD_avx2.o

FastNoiseSIMD_internal.o: $(NP)/FastNoiseSIMD_internal.cpp
	$(NFLAGS) $(NP)/FastNoiseSIMD_internal.cpp -o FastNoiseSIMD_internal.o

FastNoiseSIMD_sse2.o: $(NP)/FastNoiseSIMD_sse2.cpp
	$(NFLAGS) $(NP)/FastNoiseSIMD_sse2.cpp -o FastNoiseSIMD_sse2.o

FastNoiseSIMD_sse41.o: $(NP)/FastNoiseSIMD_sse41.cpp
	$(NFLAGS) $(NP)/FastNoiseSIMD_sse41.cpp -o FastNoiseSIMD_sse41.o

OFLAGS = g++ -c $(CFLAGSLINUX) -O3
# Main Engine


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

scommands.o: ../serversrc/MainEngine/commands.cpp
	$(OFLAGS) ../serversrc/MainEngine/commands.cpp -o scommands.o

sserver.o: ../serversrc/MainEngine/server.cpp
	$(OFLAGS) ../serversrc/MainEngine/server.cpp -o sserver.o

sFastNoiseInter.o:../serversrc/TerrainGeneration/FastNoiseSIMD_internal.cpp
	$(OFLAGS) ../serversrc/TerrainGeneration/FastNoiseSIMD_internal.cpp -o sFastNoiseInter.o
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

cmessenger.o: $(ME)/messenger.cpp
	$(OFLAGS) $(ME)/messenger.cpp -o cmessenger.o

cdrawer.o: $(ME)/drawer.cpp
	$(OFLAGS) $(ME)/drawer.cpp -o cdrawer.o
# Character
CH = ../src/Character

cgui.o: $(CH)/gui.cpp
	$(OFLAGS) $(CH)/gui.cpp -o cgui.o

cmainchar.o: $(CH)/mainchar.cpp
	$(OFLAGS) $(CH)/mainchar.cpp -o cmainchar.o

# Terrain Gen
TG = ../src/TerrainGeneration
cFastNoise.o: $(TG)/FastNoiseSIMD.cpp
	$(OFLAGS) $(TG)/FastNoiseSIMD.cpp -o cFastNoiseSIMD.o

cbspterraingen.o: $(TG)/bspterraingen.cpp
	$(OFLAGS) $(TG)/bspterraingen.cpp -o cbspterraingen.o


#IO
IO = ../src/InputHandling
cinputhandling.o: $(IO)/inputhandling.cpp
	$(OFLAGS) $(IO)/inputhandling.cpp -o cinputhandling.o

#Inventory
IT = ../src/Objects
citems.o: $(IT)/items.cpp
	$(OFLAGS) $(IT)/items.cpp -o citems.o

cobjects.o: $(IT)/objects.cpp
	$(OFLAGS) $(IT)/objects.cpp -o cobjects.o

#TextureObject

ctextureloading.o: ../src/TextureLoading/textureLoading.cpp
	$(OFLAGS) ../src/TextureLoading/textureLoading.cpp -o ctextureloading.o

#SettingsObject
csettings.o: ../src/Settings/settings.cpp
	$(OFLAGS) ../src/Settings/settings.cpp -o csettings.o

linux: $(COBJECTLIST)
	$(CC) -o linux  $(COBJECTLIST) $(CFLAGSLINUX) -O3

windows:
	cd ../src && $(WCC) -o windows.exe $(TARGET) $(CFLAGSWINDOWS)
	mv ../src/windows.exe ../build

server: $(SOBJECTLIST)
	$(CC) -o server $(SOBJECTLIST) $(CFLAGSLINUX) -O3


clean:
	rm *.o
