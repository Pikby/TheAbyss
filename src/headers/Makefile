CC = g++

CFLAGSLINUX = -lglfw3 -lGLEW -lGL -lm -ldl -lXinerama -lXrandr -lXi \
							-lXcursor -lX11 -lXxf86vm -lpthread -std=c++17 \
							-lBulletDynamics -lBulletCollision -lBulletSoftBody -lLinearMath -I /usr/local/include/bullet/

OBJECTLIST = main.o threads.o objects.o inputhandling.o objhandler.o

OFLAGS = g++ -c $(CFLAGSLINUX) -O3

default: daybreak_linux

main.o: ../src/main.cpp
	$(OFLAGS) ../src/main.cpp -o main.o
threads.o: ../src/threads.cpp
	$(OFLAGS) ../src/threads.cpp -o threads.o
objects.o: ../src/objects.cpp
	$(OFLAGS) ../src/objects.cpp -o objects.o
inputhandling.o: ../src/inputhandling.cpp
	$(OFLAGS) ../src/inputhandling.cpp -o inputhandling.o
objhandler.o: ../src/objhandler.cpp
	$(OFLAGS) ../src/objhandler.cpp -o objhandler.o
daybreak_linux: $(OBJECTLIST)
	$(CC) -o daybreak_linux $(OBJECTLIST) $(CFLAGSLINUX) -O3

clean:
	rm *.o
