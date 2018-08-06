#ifdef _WIN32
  /* See http://stackoverflow.com/questions/12765743/getaddrinfo-on-win32 */
  #ifndef _WIN32_WINNT
    #define _WIN32_WINNT 0x0501  /* Windows XP. */
  #endif
  #include <winsock2.h>
  #include <ws2tcpip.h>
#else
  /* Assume that any non-Windows platform uses POSIX-style sockets instead. */
  #include <sys/socket.h>
  #include <arpa/inet.h>
  #include <netdb.h>  /* Needed for getaddrinfo() and freeaddrinfo() */
  #include <unistd.h> /* Needed for close() */
#endif
#include <errno.h>
#include <string>
#include <iostream>
#include <fstream>
#include "include/messenger.h"

inline int pack4chars(char a, char b, char c, char d)
{
  return ((a << 24) | (b << 16) | (c << 8) | d);
}

void Messenger::setupSockets(std::string ipAddress,std::string port)
{

#ifdef _WIN32
  WSADATA wsa_data;
  int initResult;
  initResult = WSAStartup(MAKEWORD(2,2), &wsa_data);
  if (initResult != 0)
  {
      throw "WSAStartup failed, ERROR CODE: ";
      return;
  }

  // addrinfo contains a sockaddr struct
  struct addrinfo *result = NULL, *ptr = NULL, hints;
  // initialize the addrInfo structs
  ZeroMemory(&hints, sizeof(hints));  // fills a block of memory with 0s
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;

  if (getaddrinfo(ipAddress.c_str(),port.c_str(), &hints, &result) != 0)
  {
    throw "getaddrinfo failed";
    WSACleanup();
    return;
  }

  // attempt to connect to the first address returned by the call to getaddrinfo
  ptr = result;
  // create a SOCKET for the connecting server
  fd = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
  // error check to make sure it is a valid socket
  if (fd == INVALID_SOCKET)
  {
      throw "Error at socket(): " + WSAGetLastError();
      freeaddrinfo(result);
      WSACleanup();
      return;
  }
  if(connect(fd, ptr->ai_addr, (int)ptr->ai_addrlen) == SOCKET_ERROR)
  {
    throw "Error connecting to server\n";
    closesocket(fd);
    fd = INVALID_SOCKET;
    return;
  }
#else
  fd = socket(AF_INET, SOCK_STREAM, 0);
  if (fd < 0)
  {
      throw "ERROR: failed to create socket.";
      return;
  }

  sockaddr_in serveraddress;
  serveraddress.sin_family = AF_INET;
  serveraddress.sin_port = htons(stoi(port));

  inet_pton(AF_INET, ipAddress.c_str(), &(serveraddress.sin_addr));


  // connect to the server
  if (connect(fd, (sockaddr*) &serveraddress, sizeof(serveraddress)) < 0)
  {
      throw "ERROR: failed to connect to server.";
      return;
  }
  #endif
  std::cout << "Successfully connected to server" << std::endl;


}

InMessage Messenger::receiveAndDecodeMessage()
{
  int buf[5];
  receiveMessage(buf,sizeof(int)*5);
  uchar opcode = (buf[0] >> 24) & 0xFF;
  uchar ext1 = (buf[0] >> 16) & 0xFF;
  uchar ext2 = (buf[0] >> 8) & 0xFF;
  uchar ext3 = buf[0] & 0xFF;
  InMessage msg = InMessage(opcode,ext1,ext2,ext3,buf[1],buf[2],buf[3],buf[4]);
  return msg;

}

void Messenger::createChatMessage(const std::string& msg)
{
  std::cout << "making chat Message " + msg + '\n';
  OutMessage newMsg = OutMessage(100,0,0,0,0,0,0,std::make_shared<std::string> (msg));
  messageQueue.push(newMsg);
}

void Messenger::requestChunk(int x, int y, int z)
{
  int request[5];
  request[0] = 0;
  request[1] = x;
  request[2] = y;
  request[3] = z;
  request[4] = 0;
  try
  {
    sendMessage(request,sizeof(request));
  }
  catch(...)
  {
    std::cout << "ERROR: REQUESTING CHUNK, ERRNO: " << errno << "\n";
  }
}

void Messenger::sendChatMessage(std::shared_ptr<std::string> msg)
{
  std::cout << "sending chat message\n";
  int request[5];
  request[0] = pack4chars(100,0,0,0);
  request[1] = 0;
  request[2] = 0;
  request[3] = 0;
  request[4] = msg->length();
  std::cout << *msg << "\n";
  try
  {
    sendMessage(request,sizeof(request));
    sendMessage(msg->c_str(),msg->length());
  }
  catch(...)
  {
    std::cout << "ERROR: SENDING MESSAGE, ERRNO: " << errno << "\n";
  }

}
void Messenger::createMoveRequest(float x, float y, float z)
{
  OutMessage tmp = OutMessage(91,0,0,0,x,y,z,0);
  messageQueue.push(tmp);
}

void Messenger::createAddBlockRequest(int x, int y, int z, uchar id)
{
  OutMessage tmp = OutMessage(2,id,0,0,x,y,z,0);
  messageQueue.push(tmp);
}

void Messenger::requestAddBlock(int x, int y, int z, uchar id)
{
  int request[5];
  request[0] = pack4chars(2,id,0,0);
  request[1] = x;
  request[2] = y;
  request[3] = z;
  request[4] = 0;
  try
  {
    sendMessage(request,sizeof(request));
  }
  catch(...)
  {
    std::cout << "ERROR: REQUESTING BLOCK, ERRNO: " << errno << "\n";
  }
}

void Messenger::createDelBlockRequest(int x, int y, int z)
{
  OutMessage tmp = OutMessage(1,0,0,0,x,y,z,0);
  messageQueue.push(tmp);
  std::cout << "requesting del block\n";
}
void Messenger::requestDelBlock(int x, int y, int z)
{
  int request[5];
  request[0] = pack4chars(1,0,0,0);
  request[1] = x;
  request[2] = y;
  request[3] = z;
  request[4] = 0;
  try
  {
    sendMessage(request,sizeof(request));
  }
  catch(...)
  {
    std::cout << "ERROR: REQUESTING BLOCK DELETION, ERRNO: " << errno << "\n";
  }
}

void Messenger::requestMove(float x, float y, float z)
{
  int request[5];
  request[0] = pack4chars(91,0,0,0);
  request[1] = *(int*)&x;
  request[2] = *(int*)&y;
  request[3] = *(int*)&z;
  request[4] = 0;
  try
  {
    sendMessage(request, sizeof(request));
  }
  catch(...)
  {
    std::cout << "ERROR: REQUESTING BLOCK DELETION, ERRNO: " << errno << "\n";
  }
}


void Messenger::requestExit()
{
  int request[5];
  request[0] = 0xFFFFFFFF;
  sendMessage(request,sizeof(request));
  std::cout << "exit message Sent\n";
}


void Messenger::receiveMessage(void *buffer,int length)
{
  char* buf = (char*)buffer;
  int totalReceived = 0;
  while(totalReceived<length)
  {
    int curReceived = recv(fd,buf+totalReceived,length-totalReceived,0);
    if(curReceived == -1)
    {
      throw -1;
      return;
    }
    totalReceived += curReceived;
  }
}

void Messenger::sendMessage(const void* buffer, int length)
{
  char* buf = (char*)buffer;
  int totalSent = 0;
  while(totalSent<length)
  {
    int curSent = send(fd,buf+totalSent,length-totalSent,0);
    if( curSent == -1)
    {
      throw -1;
      return;
    }
    totalSent += curSent;
  }
}

void Messenger::createChunkRequest(int x, int y, int z)
{
  if(!requestMap.exists(x,y,z))
  {
    OutMessage tmp = {0,0,0,0,x,y,z,0};
    messageQueue.push(tmp);
    requestMap.add(x,y,z,true);
  }
}
