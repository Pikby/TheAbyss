#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <thread>
#include <fstream>
#include <bitset>
#include <sstream>      // std:

#define PORT 3030
#define ADDRESS "127.0.0.1"

void recievemessages(int fd) {
    // 515 chars for name + " : " + message
    char buffer[32*32*32*32];
    while (true) {
        if (recv(fd, buffer, sizeof(buffer), 0) < 0)
        {
            std::cout << "ERROR: failed to recieve message." << std::endl;
            break;
        }
        std::cout << buffer << std::endl;
    }
}

int main(int argc, char * argv[]) {

    // first get the clients name
    char name[256];
    std::cout << "Please input your name: ";
    std::cin.getline(name, 256);

    // create a socket
    int fd;
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        std::cout << "ERROR: failed to create socket." << std::endl;
        return 1;
    }
    std::cout << "Successfully created socket." << std::endl;

    sockaddr_in serveraddress;
    serveraddress.sin_family = AF_INET;
    serveraddress.sin_port = htons(PORT);
    // if there was a command line argument for the target address, use it
    // otherwise, use the default localhost address
    if (argc > 1) {
        inet_pton(AF_INET, argv[1], &(serveraddress.sin_addr));
    }else{
        inet_pton(AF_INET, ADDRESS, &(serveraddress.sin_addr));
    }

    // connect to the server
    if (connect(fd, (sockaddr*) &serveraddress, sizeof(serveraddress)) < 0) {
        std::cout << "ERROR: failed to connect to server." << std::endl;
        return 1;
    }
    std::cout << "Successfully connected to server" << std::endl;

    // send the name of the client first
    float position[3] = {150,100,100};
    if (send(fd, position, sizeof(position), 0) < 0)
    {
        std::cout << "ERROR: failed to send name to server." << std::endl;
        return 1;
    }


    while(true)
    {
      int pos[4];
      if (recv(fd, pos, sizeof(pos), 0) < 0)
      {
        std::cout << "ERROR: failed to send name to server." << std::endl;
        return 1;
      }
      std::cout << pos[0] << ":" << pos[1] << ":" << pos[2] << ":" << pos[3] << "\n";
      int length = pos[3];


      char* buffer = new char[length];
      if(recv(fd,buffer,length,0) < 0)
      {
        std::cout << "ERROR: failed to send name to server." << std::endl;
        return 1;
      }

      using namespace std;
      string chunkName = to_string(pos[0]) + '_' + to_string(pos[1]) + '_' + to_string(pos[2]) + ".dat";
      string chunkPath = chunkName;

      string x(buffer,length);
      ofstream out(chunkPath,ios::binary);
      out << x;
    }


    return 0;

}
