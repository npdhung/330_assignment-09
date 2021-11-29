/*
 *  Hung Nguyen
 *  Z1924897
 *  CSCI330 - 002
 *
 *  Assignment 9
 *
 */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <dirent.h>
#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
using namespace std;

void processClientRequest(int connSock) {
    ssize_t received;
    char path[1024], buffer[1024];

    // read a message from the client
    if ((received = read(connSock, path, sizeof(path))) < 0) {
        perror("receive");
        exit(EXIT_FAILURE);
    }
    cout << "Client request: " << path << endl;

    // open directory
    DIR *dirp = opendir(path);
    if (dirp == 0) {
        // tell client that an error occurred
        // duplicate socket descriptor into error output
        close(2);
        dup(connSock);
        perror(path);
        exit(EXIT_SUCCESS);
    }
    
    // read directory entries
    struct dirent *dirEntry;
    while ((dirEntry = readdir(dirp)) != NULL) {
        
        // if the pathname refers to a directory
        // if directory contains index.html
        // if index.html doesnot exist
        strcpy(buffer, dirEntry->d_name);
        strcat(buffer, "\n");
        if (write(connSock, buffer, strlen(buffer)) < 0) {
            perror("write");
            exit(EXIT_FAILURE);
        }
        
        // if the pathname refers to a file
        
        cout << "sent: " << buffer;
    }
    closedir(dirp);
    cout << "done with client request\n";
    close(connSock);
    exit(EXIT_SUCCESS);
}
        
int main(int argc, char *argv[]) {

    if (argc != 2) {
        cerr << "USAGE: TCPServerReadDir port\n";
        exit(EXIT_FAILURE);
    }
    
    // Create the TCP socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    // create address structures
    struct sockaddr_in server_address;  // structure for address of server
    struct sockaddr_in client_address;  // structure for address of client
    unsigned int addrlen = sizeof(client_address);

    // Construct the server sockaddr_in structure
    memset(&server_address, 0, sizeof(server_address));   /* Clear struct */
    server_address.sin_family = AF_INET;                  /* Internet/IP */
    server_address.sin_addr.s_addr = INADDR_ANY;          /* Any IP address */
    server_address.sin_port = htons(atoi(argv[1]));       /* server port */

    // Bind the socket
    int bindResult = static_cast<int>(bind(sock, (struct sockaddr *) &server_address, sizeof(server_address)));
    if (bindResult < 0) {
        perror("bind");
        exit(EXIT_FAILURE);
    }
    
    // listen: make socket passive and set length of queue
    if (listen(sock, 64) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    
    cout << "TCPServerReadDir listening on port: " << argv[1] << endl;

    // Run until cancelled
    while (true) {
        int connSock=accept(sock, (struct sockaddr *) &client_address, &addrlen);
        if (connSock < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        // fork
        if (fork()) {         // parent process
            close(connSock);
        } else {             // child process
            processClientRequest(connSock);
        }
    }
    close(sock);
    return 0;
}
