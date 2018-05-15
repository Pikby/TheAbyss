#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <thread>
#include <map>
#include <mutex>
#include <string.h>
#include <string>
#include <unistd.h>
#include <bits/stdc++.h>
#include "../headers/all.h"


#define PORT 3030


// method to be called by a thread each time a client connects
//  handles the naming/message recieving of the clients


// method to be called by a thread in main
//  handles any input from the console on the server
void handleServerCommands()
{
    char line[256];
    while(std::cin.getline(line, 256))
    {
        // quit the application if the user types quit
        if (strcmp(line, "quit") == 0)
        {
            std::cout << "Exiting the application..." << std::endl;
            exit(0);
        }
        // otherwise, just print the message
        char buffer[265];
        strcpy(buffer, "server : ");
        strcat(buffer, line);
        //sendmessages(buffer, 265, -1);
    }
}

// entry point
int main(int argc, char* argv[])
{
    // create a thread to handle server input
    std::thread serverinput(handleServerCommands);
    serverinput.detach();
    // a socket descriptor for listening for connections
    int listenfd;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd < 0)
    {
        std::cout << "ERROR: failed to create socket." << std::endl;
        return 1;
    }
    std::cout << "Successfully created socket." << std::endl;
    // bind the socket descriptor
    sockaddr_in serveraddress;
    serveraddress.sin_family = AF_INET;
    serveraddress.sin_port = htons(PORT);
    serveraddress.sin_addr.s_addr = INADDR_ANY;


    if (bind(listenfd, (sockaddr*) &serveraddress, sizeof(serveraddress)) == -1)
    {
        std::cout << "ERROR: failed to bind socket." << std::endl;
        return 1;
    }
    std::cout << "Successfully bound socket." << std::endl;
    // listen for a connection
    if (listen(listenfd, MAX_CLIENTS) == -1)
    {
        std::cout << "ERROR: socket failed to listen." << std::endl;
        return 1;
    }
    std::cout << "Listening for connections..." << std::endl;
    // keep accepting connections as long as the server is still runninng
    World curWorld;
    ClientList clientList(&curWorld);
    while (true)
    {
        // look for new connections on the listening socket
        int connfd;
        sockaddr_in clientaddress;
        socklen_t clilen = sizeof(clientaddress);
        connfd = accept(listenfd, (sockaddr*) &clientaddress, &clilen);
        if (connfd < 0)
        {
            std::cout << "ERROR: failed to connect to client." << std::endl;
            return 1;
        }
        clientList.add(connfd);
    }

    return 0;

}
