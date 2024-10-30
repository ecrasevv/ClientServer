
// https://beej.us/guide/bgnet/html/split/index.html

#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdbool.h>

#define MAX_CLIENTS 100

uint16_t PORT = 9090;
struct sockaddr_in server_addr;

// TODO -> check utils
char *ip = "192.168.1.1";

int main (int argv, const char** argc) {
    // Create server socket
    int server_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket_fd < 0) {
        perror("server socket fail");
        exit(1);
    }
    printf("server socket created.\n");

    // Config socket
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    
    if (bind(server_socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("server bind fail");
        exit(1);
    }
    printf("server bind done.\n");

    if (listen(server_socket_fd, MAX_CLIENTS) < 0) {
        perror("server listen fail.");
        exit(1);
    }
    printf("server listen done\n");

    while(true) {}

    return 0;
}
