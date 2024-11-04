
/** Client side 
* connect() -> connect the client socket to the address specified by the second arg of the SC 
*/

#include <arpa/inet.h>
#include <stdio.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define PORT        8080

// specify the ip of the server 
#define SERVER_IP        "127.0.0.1" 
#define BUFFER_SIZE      1024
#define MAX_MESSAGE_SIZE 50
struct sockaddr_in server_address;

int main (int argv, const char** argc) {
    // utils for client / server connection 
    int     server_socket_fd;
    int     connection_status;
    char    client_message[MAX_MESSAGE_SIZE];
    char    buffer[BUFFER_SIZE]; 
    ssize_t bytes_readed; 

    // setup the client socket
    int client_socket_fd;
    if ((client_socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("client socket failed.");
        exit(1);
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port   = htons(PORT);
    
    // TODO handle different errors
    if (inet_pton(AF_INET, SERVER_IP, &server_address.sin_addr.s_addr) <= 0) {
        perror("server invalid address.");
        exit(1);
    }

    if ((connection_status = connect(client_socket_fd, (struct sockaddr*)&server_address, sizeof(server_address))) == -1) {
        perror("client connection failed.");
        exit(1);
    }
    printf("client connected to the server\n");
   
    printf("print a message\n");
    fgets(client_message, MAX_MESSAGE_SIZE, stdin); 

    if (send(client_socket_fd, client_message, strlen(client_message), 0) == -1) {
        perror("client send error.");
        exit(1);
    }
 
    printf("client sended the message. [%s]\n", client_message);

    if ((bytes_readed = read(client_socket_fd, buffer, sizeof(buffer))) == -1) {
        perror("client read error.");
        exit(1);
    }

    printf("CLIENT RECIVED: %s\n", buffer);

    close(client_socket_fd);
    close(server_socket_fd);

    return 0;
}
