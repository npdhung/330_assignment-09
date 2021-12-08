/*
 *  Hung Nguyen
 *  Z1924897
 *  CSCI330 - 002
 *
 *  Assignment 9
 *  Submit for Extra credit
 *
 */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctime>
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

char specPath[1024];

void processClientRequest(int connSock) {
    ssize_t received;
    char request[1024], buffer[1024], chksyn[5];

    // read a message from the client
    if ((received = read(connSock, request, sizeof(request))) < 0) {
        perror("receive");
        exit(EXIT_FAILURE);
    }

    // check if syntax is not start with either GET or INFO
    strncpy(chksyn,&request[0],4);
    if (strcmp(chksyn,"INFO") && strcmp(chksyn,"GET ")) {
        strcpy(buffer, "Syntax error: use 'GET pathname' or 'INFO'");
        if (write(connSock, buffer, strlen(buffer)) < 0) {
            close(2);
            dup(connSock);
            perror("write");
            exit(EXIT_FAILURE);
        }
        cout << "sent: '" << buffer << "'" << endl;
        cout << "done with client request\n";
        close(connSock);
        exit(EXIT_SUCCESS);
    }
    
    // if request from client is 'INFO'
    if (!strcmp(request,"INFO")) {
        // current date/time based on current system
        time_t now = time(0);
        // convert now to string form
        char* dt = ctime(&now);
        strcpy(buffer, "Current date and time: ");
        strcat(buffer, dt);
        if (write(connSock, buffer, strlen(buffer)-1) < 0) {
            close(2);
            dup(connSock);
            perror("write");
            exit(EXIT_FAILURE);
        }
        cout << "sent: '";
        cout.write(buffer, strlen(buffer)-1);
        cout << "'" << endl;
        cout << "done with client request\n";
        close(connSock);
        exit(EXIT_SUCCESS);
    }
    
    char path[1024];
    strncpy(path, &request[4], sizeof(request)-4);
    // if the path is '/'
    // add specified root directory to the path
    if (path[0] == '/') {
        char temp[1024];
        strcpy(temp, specPath);
        strcat(temp, path);
        strcpy(path, temp);
    }
    // open directory
    DIR *dirp = opendir(path);
    
    if (dirp == 0) {
        char fileName[1024];
        
        strncpy(fileName, &request[5], sizeof(request)-5);
        if (errno == ENOENT) {
            // directory does not exist
            strcpy(buffer, "Error: ");
            strncat(buffer, &fileName[0], strlen(fileName));
            strcat(buffer, " not found");
            if (write(connSock, buffer, strlen(buffer)) < 0) {
                close(2);
                dup(connSock);
                perror("write");
                exit(EXIT_FAILURE);
            }
        }
        else
        {
            if (errno == ENOTDIR) {
                // path is not a directory
                // open file
                int fd;
                fd = open(path, O_RDONLY);
                if (fd == -1) {
                    close(2);
                    dup(connSock);
                    perror(fileName);
                    exit(EXIT_FAILURE);
                }
                // read from file
                int count;
                count = read(fd, buffer, 1024);
                if (count == -1) {
                    close(2);
                    dup(connSock);
                    perror(fileName);
                    exit(EXIT_FAILURE);
                }
                if (write(connSock, buffer, strlen(buffer)-2) < 0) {
                     close(2);
                    dup(connSock);
                    perror("write");
                    exit(EXIT_FAILURE);
                }
            }
            else {
                // error in pathname
                close(2);
                dup(connSock);
                perror(path);
                exit(EXIT_FAILURE);
            }
        }
        cout << "sent: '" << buffer << "'" << endl;
        cout << "done with client request\n";
        closedir(dirp);
        close(connSock);
        exit(EXIT_SUCCESS);
    }
    
    // read directory entries
    struct dirent *dirEntry;
    bool iflag = false;
    
    // check if the directory contains index.html
    while ((dirEntry = readdir(dirp)) != NULL)
        if (!strcmp(dirEntry->d_name,"index.html")) iflag = true;
    
    dirp = opendir(path);
    if (iflag == false) {
        while ((dirEntry = readdir(dirp)) != NULL)
            if (strcmp(dirEntry->d_name,".") && strcmp(dirEntry->d_name,"..")) {
                strcpy(buffer, dirEntry->d_name);
                strcat(buffer, " ");
                if (write(connSock, buffer, strlen(buffer)) < 0) {
                    close(2);
                    dup(connSock);
                    perror("write");
                    exit(EXIT_FAILURE);
                }
                cout << "sent: '" << buffer << "'" << endl;
            }
    }
    else {
        strcpy(buffer, "index.html");
        if (write(connSock, buffer, strlen(buffer)) < 0) {
            close(2);
            dup(connSock);
            perror("write");
            exit(EXIT_FAILURE);
        }
        cout << "sent: '" << buffer << "'" << endl;
    }
    
    closedir(dirp);
    cout << "done with client request\n";
    close(connSock);
    exit(EXIT_SUCCESS);
}
        
int main(int argc, char *argv[]) {

    if (argc != 3) {
        cerr << "USAGE: z1924897 port pathname\n";
        exit(EXIT_FAILURE);
    }

    strcpy(specPath,argv[2]);
    if (specPath[0] != '/') {
        cerr << "Error: pathname need to start with '/'" << endl;
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
