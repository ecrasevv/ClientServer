
/** Server side
 *  socket() -> create new socket
 *  bind()   -> attach socket to port
 *  listen() -> setup the socket queue
 *  accept() -> accept connections, first arg create a new socket for the client, first arg doesn't care about this call
 *  send()   -> send data
 *  recv()   -> recive data
 *  close()  -> close connection
 */

#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define PORT        8080
#define MAX_CLIENTS 5 
#define BUFFER_SIZE 1024

struct sockaddr_in server_address, client_address;

int main (int argv, const char** argc) {
    // utils for client connection
    int         client_socket_fd;
    char        buffer[BUFFER_SIZE] = {0};
    ssize_t     bytes_read;
    socklen_t   client_addrlen = sizeof(client_address);

    // utils for server
    char*       server_message = "hello from server.";
    int         server_socket_fd;
    int         number_of_connections = 0;

    if ((server_socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("server socket failed.");
        exit(1);
    }

    server_address.sin_family      = AF_INET;               /* match the socket() */
    server_address.sin_port        = htons(PORT);           /* specify port to listen on */
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);     /* the server can accept connection on any interface */

    // binding
    if ((bind(server_socket_fd, (struct sockaddr*)&server_address, sizeof(server_address))) == -1) {
        perror("server binding failed.");
        exit(1);
    }
 
    // listening 
    if ((listen(server_socket_fd, MAX_CLIENTS) == -1)) {
        perror("server listen failed.");
        exit(1);
    }
    printf("server listening for request...\n");

    for (int i = 0; i < MAX_CLIENTS; ++i ) {
        if ((client_socket_fd = accept(server_socket_fd, (struct sockaddr*)&client_address, &client_addrlen)) == -1) {
            perror("server accept client connection failed.");
            exit(1);
        }

        printf("server recived a connection!\n");

        number_of_connections++;

        if ((bytes_read = read(client_socket_fd, buffer, sizeof(buffer))) == -1) {
            perror("server read error.");
            exit(1);
        }

        printf("SERVER READ: %s\n", buffer);

        if (send(client_socket_fd, server_message, strlen(server_message), 0) == -1) {
            perror("server send error.");
            exit(1);
        }
        printf("server sended the message. [%s]\n", server_message);
    }
    printf("total connections handled by server: %d\n", number_of_connections);

    close(server_socket_fd);
    close(client_socket_fd);

    return 0;
}
