# Chunks
A [Chunk](../src/MainEngine/include/bsp.h) has size of 32x32x32 blocks,and an infinite amounts of chunks are generated in all directions.
For accessing a chunks neighbors the following definitions are used:

* X+ = right
* X- = left
* Y+ = top
* Y- = bottom
* Z+ = front
* Z- = back

Chunk data is stored as 2 byte integers, with the first byte being used for id assignment, and the second being used for metadata such as block style

### Vertex Data
Vertex data is encoded as such:

|| Position xyz | Blended Normal Vector | Absolute Normal Vector | TextureIds |Legacy| Total Size|
| :---:  | :--- |:--- | :---| :---| :--- | ---|
|Size in bits| 32+32+32     | 32 | 32 | 32  | 32  | 224 bits|

This data is then sent to the [gBuffer Vertex Shader](../src/Shaders/BSPShaders/gBuffer.vs) and dccoded for use in the [gBuffer Fragment Shader](../src/Shaders/BSPShaders/gBuffer.fs)

##### Positions
Positions are single precision floating points in model space

##### Blended Normal Vector
Blended normal vectors use an approximation technique to calculate the surrounding normal vectors and estimates the current normal vector for blending, should be used for colouring

##### Absolute Normal Vector
Gives the Absolute normal vector of a vertex, used for calculating texture coordinates

##### TextureIds
Stores the value of the 3 TextureIds of each of the triangles corners, could be done in the geometry shader

### Drawing pipeline
First past creates a 3 seperate images containing the scenes colour, normal vectors, and position, which is then passed to the [BSP Shader Fragment Shader](../src/Shaders/BSPShaders/shaderBSP.fs)


### Implementation
The same naming scheme used for chunks is used for defining neighboring blocks.
The blocks inside a chunk are implemented using a 3dArray class, which encapsulates a 1d array and allows for constant lookup in 3d space such as get(x,y,z) or set(x,y,z,id) as well as the traditional arr[number] = value

Furthermore, each chunk has access to 6 pointers to their respective neighbours

# Worlds
A [World](../src/MainEngine/include/world.h) is a data type which is used to combine a string of chunks and entities in order to create a world that can be drawn. The world is broken up into 3 classes, the main class known as World which has access to functions such as deleting and removing blocks defined in world space by finding the chunk they're located in, translating the world space coordinates to a local space coordinate, and modifying the given block inside of the given chunk.

## Drawer
The [Drawer](../src/MainEngine/include/drawer.h) is responsible for drawing all the chunks and entities in the world, as well as dealing with any effects such as shadows.

## Messenger
The [Messenger](../src/MainEngine/include/messenger.h) is responsible for all inter-server communications such as basic connection, chunk receiving, and player movement.

### Implementation
The world is implemented using a read write locked 3d nested ordered map for chunk lookup. This allows for log(x) by log(y) by log(z) lookup. This 3d map can be found as the class Map3D and is entirely thread safe as well as has a parallel list alongside the map for easy extraction.

### Messages
A [Message](../src/MainEngine/include/messages.h) is a 20 byte struct which contains the following information in this exact order
* opcode (1 byte)
* extra1 (1 byte)
* extra2 (1 byte)
* extra3 (1 byte)
* xpos   (4 bytes)
* ypos   (4 bytes)
* zpos   (4 bytes)
* length (4 bytes)

#### Definitions
* Opcode: the opcode for what ever the message is
* Extras: optional information that can be sent such as player Ids, block Ids, anything else that can fit in 1-3 bytes
* Positions: can be either a float value or a integer value, the opcode will define it
* Length: if the value is none zero, then that means a new message of size length is incoming, therefore it should be received immediately


### Opcode tables
Opcodes are an unsigned char, and all values are in decimal
These tables are implemented inside the send or receive functions found in the [Threads](../src/MainEngine/threads.cpp) class
##### Client Receive Table
| Opcode | Description | Ext1 | Ext2 | Ext3 | Position | Length |
| :---:  | :---        |:--- | :---| :---| :---:    | :---: |
| 0      | A chunk at (x,y,z) is coming in with a size (Length) | n/a  | n/a  | n/a  | Integer  | Size of incoming Chunk Data |
| 1      | Deleting block at (x,y,z) | n/a | n/a | n/a | Integer | n/a |
| 2      | Placing block at (x,y,z) | id of block  | n/a | n/a | Integer | n/a |
| 90     | Add player at (x,y,z) | id of player | n/a | n/a | Float | n/a |
| 91     | Set player position to (x,y,z) | id of player | n/a | n/a | Float | n/a |
| 92     | Set player view direction to (x,y,z) | id of player | n/a | n/a |Float|n/a
| 99     | Remove player from world | id of player | n/a | n/a | n/a | n/a |
| 100    | Receive server chat message | n/a| n/a| n/a |n/a |Size of incoming message data|
| 101    | Receive player chat message | n/a| n/a| n/a |n/a |Size of incoming message data|
| 250    | Receive ping information|
| 255    | Disconnect from server | n/a | n/a | n/a | n/a | n/a

##### Server Receive Table
| Opcode | Description | Ext1 | Ext2 | Ext3 | Position
| :---:  | :---        |:--- | :---| :---| :---:   
| 0      | Request a chunk at (x,y,z)| n/a  | n/a |n/a | Integer  
| 1      | Request a block to be deleted at (x,y,z) | n/a | n/a | n/a | Integer
| 2      | Request a block to be placed at (x,y,z) | id of block  | n/a | n/a | Integer
| 91     | Request connected player to be moved | n/a | n/a | n/a | Float |
| 92     | Request change of player view direction | n/a |n/a|n/a| Float |
| 250    | Send ping information|
| 255    | Request safe disconnect from server | n/a | n/a | n/a | n/a |
