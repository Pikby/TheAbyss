# Chunks
A chunk has size of 32x32x32 blocks,and an infinite amounts of chunks are generated in all directions.
For accessing a chunks neighbors the following definitions are used:

* X+ = right
* X- = left
* Y+ = top
* Y- = bottom
* Z+ = front
* Z- = back

### Vertex Data
Vertex data is encoded as such:

|| Position x | Position y | Position z | Normal vector | Texture Coordinates | TexID | Blocks repeated in x direction  | Blocks repeated in y direction| Total Size|
| :---:  | :---        |:--- | :---| :---| :--- |:--- | :--- | :--- | :--- |
|Size in bits| 32      | 32 | 32 | 6  | 2  |8| 8 | 8 |  128 bits or 16 bytes|

##### Positions
Positions are single precision floating points in world space

#### Normal Vector
Normal vectors use a 6 bit encoding where bits one through six determine if the block is front,back,top,bottom,left,right respectively. Can be implemented in 3 bits however this setup allows for slanted sides with proper normal vectors such as a side which is in between front and top facing;

#### Texture Coordinates
The two bits directly after the normal vectors where bit one determines if the texture is (0) bottom or (1) top, and bit two determines if the texture is (0) left or (1) right


### Implementation
The same naming scheme used for chunks is used for defining neighboring blocks.
The implementation of Chunks and Blocks can be found in MainEngine/bsp
The blocks inside a chunk are implemented using a 3dArray class, which encapsulates a 1d array and allows for constant lookup in 3d space such as get(x,y,z) or set(x,y,z,id) as well as the traditional arr[number] = value

Furthermore, each chunk has access to 6 pointers to their respective neighbours

# Worlds
A world is a data type which is used to combine a string of chunks and entities in order to create a world that can be drawn. The world is broken up into 3 classes, the main class known as World which has access to functions such as deleting and removing blocks defined in world space by finding the chunk they're located in, translating the world space coordinates to a local space coordinate, and modifying the given block inside of the given chunk.

There is a nested class inside of the World known as Drawer. Each draw loop, drawer takes a list of objects and chunks and draws then based off the camera. Drawer is responsible for calculating shadows, calculating lighting, and communicating with the GLSL shaders. 

Lastly, there is the messenger class. The messenger class is responsible for sending and receiving messages from a given server. It is used to implement TCP socket programming, and is able to compile for both winsocks and unix sockets
Using messenger.receiveAndDecodeMessage() attempts to read the current message from the server and returns a Message Struct.

### Implementation
The world is implemented using a thread-safe 3d nested ordered map for chunk lookup. This allows for log(x) by log(y) by log(z) lookup (where x is the total  of xpositions currently in memory). This 3d map can be found as the class Map3D and is entirely thread safe as well as has a parallel list alongside the map for easy extraction; Although a an unordered map would give faster lookup speed, the ordered map allows for the ability to define a min and max value and return all chunks defined between those values. For example call findAll((0,0,0),(10,10,10)) to return a list of size 10^3 all of chunks in that range

### Messages
A message is a 20 byte class which contains the following information in this exact order
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
##### Client Receive Table
| Opcode | Description | Ext1 | Ext2 | Ext3 | Position | Length |
| :---:  | :---        |:--- | :---| :---| :---:    | :---: |
| 0      | A chunk at (x,y,z) is coming in with a size (Length) | n/a  | n/a  | n/a  | Integer  | Size of incoming Chunk Data |
| 1      | Deleting block at (x,y,z) | n/a | n/a | n/a | Integer | n/a |
| 2      | Placing block at (x,y,z) | id of block  | n/a | n/a | Integer | n/a |
| 90     | Add player at (x,y,z) | id of player | n/a | n/a | Float | n/a |
| 91     | Set player position to (x,y,z) | id of player | n/a | n/a | Float | n/a |
| 99     | Remove player from world | id of player | n/a | n/a | n/a | n/a |
| 255    | Disconnect from server | n/a | n/a | n/a | n/a | n/a

##### Server Receive Table
| Opcode | Description | Ext1 | Ext2 | Ext3 | Position
| :---:  | :---        |:--- | :---| :---| :---:   
| 0      | Request a chunk at (x,y,z)| n/a  | n/a |n/a | Integer  
| 1      | Request a block to be deleted at (x,y,z) | n/a | n/a | n/a | Integer
| 2      | Request a block to be placed at (x,y,z) | id of block  | n/a | n/a | Integer
| 91     | Request connected player to be moved | n/a | n/a | n/a | Float
| 255    | Request safe disconnect from server | n/a | n/a | n/a | n/a |
