CLIENTOBJECTPATH = ../bin/client/
CLIENTSRCPATH = ../src/

SERVEROBJECTPATH = ../bin/server/
SERVERSRCPATH = ../serversrc/

NOISEOBJECTPATH = ../bin/noise/
NOISESRCPATH = ../serversrc/TerrainGeneration/FastNoiseSIMD/


CC = g++
WCC = /mnt/WorkSpace/MXE/mxe/usr/bin/x86_64-w64-mingw32.static.posix-g++


WINSRC = $(wildcard ../src/**/*.cpp)
CLIENTOBJECTS = $(patsubst %.cpp, %.o, $(wildcard ../src/**/*.cpp))
CLIENTOBJECTSOURCES = $(CLIENTOBJECTS:$(CLIENTSRCPATH)%=%)

SERVEROBJECTS = $(patsubst %.cpp, %.o, $(wildcard ../serversrc/**/*.cpp))
SERVEROBJECTSOURCES = $(SERVEROBJECTS:$(SERVERSRCPATH)%=%)

NOISEOBJECTS = $(patsubst %.cpp, %.o, $(wildcard ../serversrc/TerrainGeneration/FastNoiseSIMD/*.cpp))
NOISEOBJECTSOURCES = $(NOISEOBJECTS:$(NOISESRCPATH)%=%)




CLIENTFLAGSLINUX  = -I/usr/local/include/freetype2 -I/usr/include/libpng12 \
					-lglfw3 -lGLEW -lGL -lm -ldl -lXinerama -lXrandr -lXi \
					-lXcursor -lX11 -lXxf86vm -lpthread -std=c++17 \
					-L/usr/local/lib -lfreetype -lstdc++fs -lboost_system -lboost_filesystem \
					-lCEGUIOpenGLRenderer-0 -lCEGUIBase-0 -O3

SERVERFLAGSLINUX = -lpthread -std=c++17 -lstdc++fs -lboost_system -lboost_filesystem -O3\

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
	@echo Compiling terrain $<
	@$(CC) $(NOISEFLAGSLINUX) -c $< -o $@

$(SERVEROBJECTPATH)%.o: $(SERVERSRCPATH)%.cpp
	@echo Compiling $<
	@$(CC) $(SERVERFLAGSLINUX) -c $< -o $@



$(CLIENTOBJECTPATH)%.o: $(CLIENTSRCPATH)%.cpp
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
	@echo $(NOISEOBJECTS)
	@echo $(NOISEOBJECTSOURCES)
