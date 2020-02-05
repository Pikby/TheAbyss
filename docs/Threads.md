There are 6 different [threads](../src/MainEngine/include/threads.h) running during gameplay not including the main thread. There are render,cleanup,send,receive,logic,and chunk building threads.

####Render Thread
Is responsible for finding which chunks the player needs to receive from the server depending on there location. The thread then sends the chunks it needs to the send thread to send chunk requests to the server

####Cleanup Thread
Is responsible for intermittently checking for chunks that the player should no longer be in memory, and flags them for deletion.

####Send Thread
Is responsible for sending any requests the client wants to make to the server, such as movement, block placement, messages and the like.

####Receive Thread
Is responsible for receiving any information from the server and updating the world accordingly.

#####Logic Thread
Is responsible for updating and sending any relative data during its refresh. Examples including sending current player position

#####Build Thread
Is responsible for doing the CPU meshing of the chunk data, which then sends the chunk data back to the main Thread.

#####Main Thread
The main thread goes is the sole thread with openGL context, and is the only thread used for drawing. It will draw the GUI, the world, and all its objects every frame.
