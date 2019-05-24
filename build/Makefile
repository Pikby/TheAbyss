CLIENTOBJECTPATH = ../bin/client/
CLIENTSRCPATH = ../src/

SERVEROBJECTPATH = ../bin/server/
SERVERSRCPATH = ../serversrc/

NOISEOBJECTPATH = ../bin/noise/
NOISESRCPATH = ../serversrc/TerrainGeneration/FastNoiseSIMD/



CPPV = -std=c++17

CC = g++
WCC = /mnt/WorkSpace/MXE/mxe/usr/bin/x86_64-w64-mingw32.static.posix-g++


WINSRC = $(wildcard ../src/**/*.cpp)

CLIENTOBJECTS = $(patsubst %.cpp, %.o, $(shell find ../src/ -name "*.cpp"))
CLIENTOBJECTSOURCES = $(CLIENTOBJECTS:$(CLIENTSRCPATH)%=%)


SERVEROBJECTS = $(patsubst %.cpp, %.o, $(wildcard ../serversrc/**/*.cpp))
SERVEROBJECTSOURCES = $(SERVEROBJECTS:$(SERVERSRCPATH)%=%)

NOISEOBJECTS = $(patsubst %.cpp, %.o, $(wildcard ../serversrc/TerrainGeneration/FastNoiseSIMD/*.cpp))
NOISEOBJECTSOURCES = $(NOISEOBJECTS:$(NOISESRCPATH)%=%)



LIBRARYLIST = glfw3 freetype2 glew
CLIENTFLAGSLINUX = $(shell pkg-config --cflags $(LIBRARYLIST)) $(shell pkg-config --libs $(LIBRARYLIST)) \
									 -lpthread -lboost_system -lboost_filesystem  $(CPPV) -O3



SERVERFLAGSLINUX = -lpthread $(CPPV) -lstdc++fs -lboost_system -lboost_filesystem -O3\

#core-avx2 is very volatile flag, only use it on files that can use it
NOISEFLAGSLINUX = -std=c++17 -march=core-avx2

CFLAGSWINDOWS =-I/usr/local/include/freetype2 -I/usr/include/libpng12  \
		-mwindows -lmingw32 -lGLEW \
		-lopengl32 -lm `/mnt/WorkSpace/MXE/mxe/usr/bin/x86_64-w64-mingw32.static.posix-pkg-config --libs glfw3` \
		-lglu32 -lglew32 -lglew32s -std=c++17 -lpng -pthread -lz -lfreetype \
		`/mnt/WorkSpace/MXE/mxe/usr/bin/x86_64-w64-mingw32.static.posix-pkg-config --libs freetype2` \
		`/mnt/WorkSpace/MXE/mxe/usr/bin/x86_64-w64-mingw32.static.posix-pkg-config --libs CEGUI-0` \
		`/mnt/WorkSpace/MXE/mxe/usr/bin/x86_64-w64-mingw32.static.posix-pkg-config --libs CEGUI-0-NULL` \
		`/mnt/WorkSpace/MXE/mxe/usr/bin/x86_64-w64-mingw32.static.posix-pkg-config --libs CEGUI-0-OPENGL` \
		`/mnt/WorkSpace/MXE/mxe/usr/bin/x86_64-w64-mingw32.static.posix-pkg-config --libs CEGUI-0-OPENGL3` \
		-lboost_filesystem-mt -lboost_system-mt \
		-lboost_serialization-mt \
		-lboost_thread_win32-mt \
		-lboost_system-mt \
		-lboost_chrono-mt \
		-lboost_context-mt \
		-lCEGUIBase-0_Static -lCEGUIBase-0_Static -lCEGUILibXMLParser_Static \
		-lCEGUICoreWindowRendererSet_Static -lCEGUIFreeImageImageCodec_Static


$(NOISEOBJECTPATH)%.o: $(NOISESRCPATH)%.cpp
	@rm -f $@
	@mkdir -p $@
	@rmdir $@
	@echo Compiling terrain $<
	@$(CC) $(NOISEFLAGSLINUX) -c $< -o $@

$(SERVEROBJECTPATH)%.o: $(SERVERSRCPATH)%.cpp
	@rm -f $@
	@mkdir -p $@
	@rmdir $@
	@echo Compiling $<
	@$(CC) $(SERVERFLAGSLINUX) -c $< -o $@



$(CLIENTOBJECTPATH)%.o: $(CLIENTSRCPATH)%.cpp
	@rm -f $@
	@mkdir -p $@
	@rmdir $@
	@echo Compiling $<
	@$(CC) $(CLIENTFLAGSLINUX) -c $< -o $@

linux: $(addprefix $(CLIENTOBJECTPATH), $(CLIENTOBJECTSOURCES))
	@echo Linking objects together, outputting: linux
	@$(CC) $(addprefix $(CLIENTOBJECTPATH), $(CLIENTOBJECTSOURCES)) $(CLIENTFLAGSLINUX) -o linux

server: $(addprefix $(SERVEROBJECTPATH), $(SERVEROBJECTSOURCES)) $(addprefix $(NOISEOBJECTPATH), $(NOISEOBJECTSOURCES))
	@echo Linking objects together, outputting: server
	@$(CC)  $(addprefix $(SERVEROBJECTPATH), $(SERVEROBJECTSOURCES)) $(addprefix $(NOISEOBJECTPATH), $(NOISEOBJECTSOURCES)) $(SERVERFLAGSLINUX) -o server

windows:
	$(WCC) -o windows.exe $(WINSRC) $(CFLAGSWINDOWS)

cleanclient:
	cd ../bin/client && find . -type f -name '*.o' -delete

cleanserver:
	cd ../bin/server && find . -type f -name '*.o' -delete
	cd ../bin/noise && find . -type f -name '*.o' -delete

test:
	@echo Flags are $(CLIENTFLAGSLINUX)
