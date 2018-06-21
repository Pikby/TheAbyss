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
#include <string>
#include <iostream>
#include <fstream>
#include "include/messenger.h"
#define PORT 3030

inline int pack4chars(char a, char b, char c, char d)
{
  return ((a << 24) | (b << 16) | (c << 8) | d);
}

void Messenger::setupSockets(std::string ipAddress)
{
  std::ofstream error("outputlog.txt");
  #ifdef _WIN32
  error << "Doing windows shit\n";
  WSADATA wsa_data;
  int initResult;
  initResult = WSAStartup(MAKEWORD(2,2), &wsa_data);
  if (initResult != 0)
  {
      error << "WSAStartup failed, ERROR CODE: " << initResult << std::endl;
      return;
  }

  // addrinfo contains a sockaddr struct
  struct addrinfo *result = NULL, *ptr = NULL, hints;
  // initialize the addrInfo structs
  ZeroMemory(&hints, sizeof(hints));  // fills a block of memory with 0s
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;

  if (getaddrinfo(ipAddress.c_str(),"3030", &hints, &result) != 0)
  {
    error << "getaddrinfo failed" << std::endl;
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
      error << "Error at socket(): " << WSAGetLastError() << std::endl;
      freeaddrinfo(result);
      WSACleanup();
      return;
  }
  if(connect(fd, ptr->ai_addr, (int)ptr->ai_addrlen) == SOCKET_ERROR)
  {
    error << "Error connecting to server\n";
    closesocket(fd);
    fd = INVALID_SOCKET;
  }


  #else
  error << "Doing linux shit\n";
  fd = socket(AF_INET, SOCK_STREAM, 0);
  if (fd < 0) {
      error << "ERROR: failed to create socket." << std::endl;
      return;
  }
  error << "Successfully created socket." << std::endl;

  sockaddr_in serveraddress;
  serveraddress.sin_family = AF_INET;
  serveraddress.sin_port = htons(PORT);

  inet_pton(AF_INET, ipAddress.c_str(), &(serveraddress.sin_addr));


  // connect to the server
  if (connect(fd, (sockaddr*) &serveraddress, sizeof(serveraddress)) < 0)
  {
      error << "ERROR: failed to connect to server." << std::endl;
      return;
  }
  #endif
  error << "Successfully connected to server" << std::endl;


}

Message Messenger::receiveAndDecodeMessage()
{
  int buf[5];
  receiveMessage(buf,sizeof(int)*5);
  uchar opcode = (buf[0] >> 24) & 0xFF;
  uchar ext1 = (buf[0] >> 16) & 0xFF;
  uchar ext2 = (buf[0] >> 8) & 0xFF;
  uchar ext3 = buf[0] & 0xFF;
  Message msg = {opcode,ext1,ext2,ext3,buf[1],buf[2],buf[3],buf[4]};
  return msg;

}



void Messenger::requestChunk(int x, int y, int z)
{
  int request[4];
  request[0] = 0;
  request[1] = x;
  request[2] = y;
  request[3] = z;


  if(send(fd,(char*)request,4*sizeof(int),0)<0)
  {
    std::cout << "Failed to send message to server\n";
    return;
  }
  //std::cout << "Requesting chunk" << x << ":" << y << ":" << z << "\n";
}


void Messenger::createMoveRequest(float x, float y, float z)
{
  Message tmp = {91,0,0,0,*(int*)&x,*(int*)&y,*(int*)&z,0};
  messageQueue.push(tmp);
}

void Messenger::createAddBlockRequest(int x, int y, int z, uchar id)
{
  Message tmp = {2,id,0,0,x,y,z,0};
  messageQueue.push(tmp);
}

void Messenger::requestAddBlock(int x, int y, int z, uchar id)
{
  int request[4];
  request[0] = pack4chars(2,id,0,0);
  request[1] = x;
  request[2] = y;
  request[3] = z;
  if(send(fd,(char*)request,4*sizeof(int),0)<0)
  {
    std::cout << "Failed to send message to server\n";
    return;
  }
}

void Messenger::createDelBlockRequest(int x, int y, int z)
{
  Message tmp = {1,0,0,0,x,y,z,0};
  messageQueue.push(tmp);

}
void Messenger::requestDelBlock(int x, int y, int z)
{
  int request[4];
  request[0] = pack4chars(1,0,0,0);
  request[1] = x;
  request[2] = y;
  request[3] = z;

  if(send(fd,(char*)request,4*sizeof(int),0)<0)
  {
    std::cout << "Failed to send message to server\n";
    return;
  }
}

void Messenger::requestMove(float x, float y, float z)
{

  int request[4];
  request[0] = pack4chars(91,0,0,0);
  request[1] = *(int*)&x;
  request[2] = *(int*)&y;
  request[3] = *(int*)&z;
  if(send(fd,(char*)request,4*sizeof(int),0)<0)
  {
    std::cout << "Failed to send message to server\n";
    return;
  }
}


void Messenger::requestExit()
{
  int request[4];
  request[0] = 0xFFFFFFFF;
  if(send(fd,(char*)request,4*sizeof(int),0)<0)
  {
    std::cout << "Failed to send exit message to server\n";
    return;
  }
  std::cout << "exit message Sent\n";
}


void Messenger::receiveMessage(void *buffer,int length)
{
  char* buf = (char*)buffer;
  int totalReceived = 0;
  while(totalReceived<length)
  {
    int curReceived = recv(fd,buf+totalReceived,length-totalReceived,0);
    if(curReceived < 0)
    {
      std::cout << "ERROR: receiving message." << std::endl;
    }
    totalReceived += curReceived;

  }
}

void Messenger::createChunkRequest(int x, int y, int z)
{
  if(!requestMap.exists(x,y,z))
  {
    Message tmp = {0,0,0,0,x,y,z,0};
    messageQueue.push(tmp);
    requestMap.add(x,y,z,true);
}
}
