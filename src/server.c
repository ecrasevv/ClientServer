#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdbool.h>

#define PORT                       25325
#define MAX_CLIENTS                5 
#define CLIENT_MESSAGE_BUFFER_SIZE 1024
#define CLIENT_IP_BUFFER_SIZE      20

const char* BANNED_WORD            = "test";
const char* WARNING_SERVER_MESSAGE = "Calm Down!";

bool Parse_Client_Message(const char* message);

struct sockaddr_in server_address, client_address;

int main (int argv, const char** argc) {
    // utils for client connection
    int         client_socket_fd;
    char        client_message_buffer[CLIENT_MESSAGE_BUFFER_SIZE];
    char        client_ip_address[CLIENT_IP_BUFFER_SIZE];
    ssize_t     bytes_read;
    socklen_t   client_addrlen = sizeof(client_address);

    // utils for server
    char*       server_message = "hello from server.";
    int         server_socket_fd;
    int         number_of_connections = 0;

    if ((server_socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("server socket failed");
        exit(1);
    }

    server_address.sin_family      = AF_INET;               /* match the socket() */
    server_address.sin_port        = htons(PORT);           /* specify port to listen on */
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);     /* the server can accept connection on any interface */

    // binding
    if ((bind(server_socket_fd, (struct sockaddr*)&server_address, sizeof(server_address))) == -1) {
        perror("server binding failed");
        exit(1);
    }

    // listening 
    if ((listen(server_socket_fd, MAX_CLIENTS) == -1)) {
        perror("server listen failed");
        exit(1);
    }
    printf("[+] server listening on port %d\n", PORT);

    // handle connections
    for (int i = 0; i < MAX_CLIENTS; ++i ) {
        if ((client_socket_fd = accept(server_socket_fd, (struct sockaddr*)&client_address, &client_addrlen)) == -1) {
            perror("server accept client connection failed");
            exit(1);
        }

        inet_ntop(AF_INET, &client_address, client_ip_address, CLIENT_IP_BUFFER_SIZE);
        printf("[+] server recived a connection from %s!\n", client_ip_address);
        number_of_connections++;

        // read
        if ((bytes_read = read(client_socket_fd, client_message_buffer, CLIENT_MESSAGE_BUFFER_SIZE)) == -1) {
            perror("server read error");
            exit(1);
        }

        client_message_buffer[strcspn(client_message_buffer, "\n")] = 0;
        printf("[<-] server read: %s [%zd]\n", client_message_buffer, bytes_read);

        // send
        if (Parse_Client_Message(client_message_buffer)){
            printf("[!] client said the BANNED_WORD!\n");
            if (send(client_socket_fd, WARNING_SERVER_MESSAGE, strlen(WARNING_SERVER_MESSAGE), 0) == -1) {
                perror("server send error");
                exit(1);
            }
            close(client_socket_fd);
        } else if (bytes_read == 0) {
            printf("[+] the client sent a blank message or crash\n");
        } else {
            if (send(client_socket_fd, server_message, strlen(server_message), 0) == -1) {
                perror("server send error");
                exit(1);
            }
            printf("[->] server sended the message. [%s]\n", server_message);
        }
    }

    close(server_socket_fd);
    close(client_socket_fd);

    return 0;
}

bool Parse_Client_Message (const char* message) {
    if (strstr(message, BANNED_WORD) != NULL) {
        return true; 
    } else {
        return false;
    }
}

