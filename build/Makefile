# the compiler: gcc for C program, define as g++ for C++
default: linux
CC = g++
WCC = /mnt/WorkSpace/MXE/mxe/usr/bin/i686-w64-mingw32.static.posix-g++
WPATH = /mnt/WorkSpace/MXE/mxe/usr/i686-w64-mingw32.static.posix-g++

WCC = /mnt/WorkSpace/MXE/mxe/usr/bin/i686-w64-mingw32.static.posix-g++
CFLAGSLINUX  = -I/usr/local/include/freetype2 -I/usr/include/libpng12 \
					-lglfw3 -lGLEW -lGL -lm -ldl -lXinerama -lXrandr -lXi \
					-lXcursor -lX11 -lXxf86vm -lpthread -std=c++17 \
					-L/usr/local/lib -lfreetype -lstdc++fs -lboost_system -lboost_filesystem \
					-lCEGUIOpenGLRenderer-0 -lCEGUIBase-0

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



COBJECTLIST = $(C)settings.o $(C)drawer.o $(C)textureloading.o $(C)main.o $(C)world.o $(C)messenger.o \
							$(C)objects.o $(C)threads.o \
							$(C)bsp.o $(C)gui.o $(C)mainchar.o $(C)inputhandling.o \
							$(C)items.o
SOBJECTLIST = $(S)main.o $(S)world.o $(S)bsp.o $(S)bspterraingen.o  \
 							$(S)items.o $(S)clients.o $(S)commands.o $(S)server.o $(FASTNOISEOBJECTS)

F = ../bin/noise
FASTNOISEOBJECTS = $(F)/FastNoiseSIMD.o $(F)/FastNoiseSIMD_avx2.o $(F)/FastNoiseSIMD_internal.o \
									 $(F)/FastNoiseSIMD_sse2.o $(F)/FastNoiseSIMD_sse41.o
NP =  ../serversrc/TerrainGeneration/FastNoiseSIMD
NFLAGS = g++ -std=c++17 -march=core-avx2 -c


$(F)/FastNoiseSIMD.o: $(NP)/FastNoiseSIMD.cpp
	$(NFLAGS) $(NP)/FastNoiseSIMD.cpp -o $(F)/FastNoiseSIMD.o

$(F)/FastNoiseSIMD_avx2.o: $(NP)/FastNoiseSIMD_avx2.cpp
	$(NFLAGS) $(NP)/FastNoiseSIMD_avx2.cpp -o $(F)/FastNoiseSIMD_avx2.o

$(F)/FastNoiseSIMD_internal.o: $(NP)/FastNoiseSIMD_internal.cpp
	$(NFLAGS) $(NP)/FastNoiseSIMD_internal.cpp -o $(F)/FastNoiseSIMD_internal.o

$(F)/FastNoiseSIMD_sse2.o: $(NP)/FastNoiseSIMD_sse2.cpp
	$(NFLAGS) $(NP)/FastNoiseSIMD_sse2.cpp -o $(F)/FastNoiseSIMD_sse2.o

$(F)/FastNoiseSIMD_sse41.o: $(NP)/FastNoiseSIMD_sse41.cpp
	$(NFLAGS) $(NP)/FastNoiseSIMD_sse41.cpp -o $(F)/FastNoiseSIMD_sse41.o

OFLAGS = g++ -c $(CFLAGSLINUX) -O3
# Main Engine

S = ../bin/server/
# Server
$(S)main.o: ../serversrc/MainEngine/main.cpp
	$(OFLAGS) ../serversrc/MainEngine/main.cpp -o $(S)main.o

$(S)world.o: ../serversrc/MainEngine/world.cpp
	$(OFLAGS) ../serversrc/MainEngine/world.cpp -o $(S)world.o

$(S)bsp.o: ../serversrc/MainEngine/bsp.cpp
	$(OFLAGS) ../serversrc/MainEngine/bsp.cpp -o $(S)bsp.o

$(S)bspterraingen.o: ../serversrc/TerrainGeneration/bspterraingen.cpp
	$(OFLAGS) ../serversrc/TerrainGeneration/bspterraingen.cpp -o $(S)bspterraingen.o


$(S)items.o: ../serversrc/Inventory/items.cpp
	$(OFLAGS) ../serversrc/Inventory/items.cpp -o $(S)items.o

$(S)clients.o: ../serversrc/MainEngine/clients.cpp
	$(OFLAGS) ../serversrc/MainEngine/clients.cpp -o $(S)/clients.o

$(S)commands.o: ../serversrc/MainEngine/commands.cpp
	$(OFLAGS) ../serversrc/MainEngine/commands.cpp -o $(S)commands.o

$(S)server.o: ../serversrc/MainEngine/server.cpp
	$(OFLAGS) ../serversrc/MainEngine/server.cpp -o $(S)server.o

$(S)FastNoiseInter.o:../serversrc/TerrainGeneration/FastNoiseSIMD_internal.cpp
	$(OFLAGS) ../serversrc/TerrainGeneration/FastNoiseSIMD_internal.cpp -o $(S)FastNoiseInter.o
# Client

C = ../bin/client/
ME = ../src/MainEngine
$(C)main.o: $(ME)/main.cpp
	 $(OFLAGS) $(ME)/main.cpp -o $(C)main.o

$(C)world.o: $(ME)/world.cpp
	$(OFLAGS) $(ME)/world.cpp -o $(C)world.o

$(C)threads.o: $(ME)/threads.cpp
	$(OFLAGS) $(ME)/threads.cpp -o $(C)threads.o

$(C)bsp.o: $(ME)/bsp.cpp
	$(OFLAGS) $(ME)/bsp.cpp -o $(C)bsp.o

$(C)messenger.o: $(ME)/messenger.cpp
	$(OFLAGS) $(ME)/messenger.cpp -o $(C)messenger.o

$(C)drawer.o: $(ME)/drawer.cpp
	$(OFLAGS) $(ME)/drawer.cpp -o $(C)drawer.o
# Character
CH = ../src/Character

$(C)gui.o: $(CH)/gui.cpp
	$(OFLAGS) $(CH)/gui.cpp -o $(C)gui.o

$(C)mainchar.o: $(CH)/mainchar.cpp
	$(OFLAGS) $(CH)/mainchar.cpp -o $(C)mainchar.o

# Terrain Gen
TG = ../src/TerrainGeneration
$(C)FastNoise.o: $(TG)/FastNoiseSIMD.cpp
	$(OFLAGS) $(TG)/FastNoiseSIMD.cpp -o $(C)FastNoiseSIMD.o

$(C)bspterraingen.o: $(TG)/bspterraingen.cpp
	$(OFLAGS) $(TG)/bspterraingen.cpp -o $(C)bspterraingen.o


#IO
IO = ../src/InputHandling
$(C)inputhandling.o: $(IO)/inputhandling.cpp
	$(OFLAGS) $(IO)/inputhandling.cpp -o $(C)inputhandling.o

#Inventory
IT = ../src/Objects
$(C)items.o: $(IT)/items.cpp
	$(OFLAGS) $(IT)/items.cpp -o $(C)items.o

$(C)objects.o: $(IT)/objects.cpp
	$(OFLAGS) $(IT)/objects.cpp -o $(C)objects.o

#TextureObject

$(C)textureloading.o: ../src/TextureLoading/textureLoading.cpp
	$(OFLAGS) ../src/TextureLoading/textureLoading.cpp -o $(C)textureloading.o

#SettingsObject
$(C)settings.o: ../src/Settings/settings.cpp
	$(OFLAGS) ../src/Settings/settings.cpp -o $(C)settings.o

linux: $(COBJECTLIST)
	$(CC) -o linux  $(COBJECTLIST) $(CFLAGSLINUX) -O3

windows:
	cd ../src && $(WCC) -o windows.exe $(TARGET) $(CFLAGSWINDOWS)
	mv ../src/windows.exe ../build

server: $(SOBJECTLIST)
	$(CC) -o server $(SOBJECTLIST) $(CFLAGSLINUX) -O3


cleanserver:
	cd ../bin/server && rm *.o

cleanclient:
	cd ../bin/client && rm *.o

cleannoise:
	cd ../bin/noise && rm *.o
