CLIENTOBJECTPATH = ../bin/client/
CLIENTSRCPATH = ../src/

SERVEROBJECTPATH = ../bin/server/
SERVERSRCPATH = ../serversrc/

CC = g++

CLIENTOBJECTS = $(patsubst %.cpp, %.o, $(wildcard ../src/**/*.cpp))
CLIENTOBJECTSOURCES = $(CLIENTOBJECTS:$(CLIENTSRCPATH)%=%)

SERVEROBJECTS = $(patsubst %.cpp, %.o, $(wildcard ../serversrc/**/*.cpp))\
 								$(patsubst %.cpp, %.o, $(wildcard ../serversrc/TerrainGeneration/FastNoiseSIMD/*.cpp))
SERVEROBJECTSOURCES = $(SERVEROBJECTS:$(SERVERSRCPATH)%=%)

CLIENTFLAGSLINUX  = -I/usr/local/include/freetype2 -I/usr/include/libpng12 \
					-lglfw3 -lGLEW -lGL -lm -ldl -lXinerama -lXrandr -lXi \
					-lXcursor -lX11 -lXxf86vm -lpthread -std=c++17 \
					-L/usr/local/lib -lfreetype -lstdc++fs -lboost_system -lboost_filesystem \
					-lCEGUIOpenGLRenderer-0 -lCEGUIBase-0 -march=core-avx2


$(SERVEROBJECTPATH)%.o: $(SERVERSRCPATH)%.cpp
	$(CC) $(CLIENTFLAGSLINUX) -c $< -o $@


$(CLIENTOBJECTPATH)%.o: $(CLIENTSRCPATH)%.cpp
	$(CC) $(CLIENTFLAGSLINUX) -c $< -o $@

linux: $(addprefix $(CLIENTOBJECTPATH), $(CLIENTOBJECTSOURCES))
	$(CC) $(addprefix $(CLIENTOBJECTPATH), $(CLIENTOBJECTSOURCES)) $(CLIENTFLAGSLINUX) -o linux

server:  $(addprefix $(SERVEROBJECTPATH), $(SERVEROBJECTSOURCES))
	$(CC)  $(addprefix $(SERVEROBJECTPATH), $(SERVEROBJECTSOURCES)) $(CLIENTFLAGSLINUX) -o server


cleanclient:
	cd ../bin/client && find . -type f -name '*.o' -delete

cleanserver:
	cd ../bin/server && find . -type f -name '*.o' -delete

test:
	@echo $(SERVEROBJECTSOURCES)
	@echo $(CLIENTOBJECTSOURCES)
