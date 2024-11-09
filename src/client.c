#include <arpa/inet.h>
#include <stdint.h>
#include <stdio.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX_PORT                65535
#define MAX_IP                  20    /* 15 + '\0' */
#define BUFFER_SIZE             1024
#define CLIENT_MESSAGE_SIZE     50

struct sockaddr_in server_address;

typedef struct {
    char        server_ip[MAX_IP];
    uint16_t    server_port;
    int         timeout; 
} config;

void configure_client (config*, struct sockaddr_in*);

int main (int argv, const char** argc) {
    int         client_socket_fd;
    int         connection_status;
    char        client_message[CLIENT_MESSAGE_SIZE];
    char        buffer[BUFFER_SIZE]; 
    ssize_t     bytes_readed; 

    // setup the client socket
    if ((client_socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("client socket failed");
        exit(1);
    }

    // config the client
    config client_config;
    configure_client(&client_config, &server_address);

    // connect
    if ((connection_status = connect(client_socket_fd, (struct sockaddr*)&server_address, sizeof(server_address))) == -1) {
        perror("client connection failed");
        exit(1);
    }
    printf("[+] client connected to the server\n");

    // send
    printf("[+] print a message: ");
    if (fgets(client_message, CLIENT_MESSAGE_SIZE, stdin) == NULL) {
        perror("client send message failed");
        exit(1);
    }

    if (send(client_socket_fd, client_message, strlen(client_message), 0) == -1) {
        perror("client send error");
        exit(1);
    }

    client_message[strcspn(client_message, "\n")] = 0;
    printf("[->] client sended the message. [%s] \n", client_message);

    // read
    if ((bytes_readed = read(client_socket_fd, buffer, sizeof(buffer))) == -1) {
        perror("client read error");
        exit(1);
    }
    printf("[<-] client recived: %s\n", buffer);

    close(client_socket_fd);

    return 0;
}

void configure_client (config* client_config, struct sockaddr_in* server_address) {
    char    port_buffer[MAX_PORT];
    char    ip_buffer[MAX_IP];
    char*   endptr;

    server_address->sin_family = AF_INET;

    printf("[+] configure client\n");

    // ip
    printf("[+] server ip: ");

    if (fgets(ip_buffer, MAX_IP, stdin) == NULL) {
        perror("client failed server_ip config");
        exit(1);
    }
    ip_buffer[strcspn(ip_buffer, "\n")] = 0;     /* remove '\n' */
    strcpy(client_config->server_ip, ip_buffer);

    if (inet_pton(AF_INET, client_config->server_ip, &server_address->sin_addr.s_addr) <= 0) {
        perror("server invalid address");
        exit(1);
    }

    // port
    printf("[+] server port: ");

    if (fgets(port_buffer, MAX_PORT, stdin) == NULL) {
        perror("client failed server_port config");
        exit(1);
    }
    client_config->server_port  = strtoul(port_buffer, &endptr, 10);
    server_address->sin_port    = htons(client_config->server_port);
}








