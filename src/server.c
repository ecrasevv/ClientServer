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
#include <assert.h>
#include <dirent.h>

#define PORT                       7999
#define MAX_CLIENTS                5 
#define CLIENT_MESSAGE_BUFFER_SIZE 1024
#define CLIENT_IP_BUFFER_SIZE      20

const char* BLANK_MSG_FROM_CLIENT   = "you sent a blank message, terminating";
const char* BANNED_WORD             = "test";
const char* WARNING_SERVER_MESSAGE  = "Calm Down!";
char        SERVER_FOLDER_PATH[100] = "/home/ecrasevvv/LearnNewStuff/C/ClientServer/server_files/";

struct sockaddr_in server_address, client_address;
struct dirent* server_directory;

void configure_server(int*);
void handle_connections(int);
bool parse_client_message(const char*);
char* parse_and_check_for_file(const char*);
void append_char_to_str(char*, char);
char* get_file_content(char*);

int main (int argv, const char** argc)
{
    // config the server
    int server_socket_fd;
    configure_server(&server_socket_fd);

    printf("[+] server listening on port %d\n", PORT);

    // clients connections
    handle_connections(server_socket_fd);

    close(server_socket_fd);

    return 0;
}

void configure_server (int *server_socket_fd)
{
    if ((*server_socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("server socket failed");
        exit(1);
    }

    server_address.sin_family      = AF_INET;               
    server_address.sin_port        = htons(PORT);           
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);     

    // binding
    if ((bind(*server_socket_fd, (struct sockaddr*)&server_address, sizeof(server_address))) == -1) {
        perror("server binding failed");
        exit(1);
    }

    // listening 
    if ((listen(*server_socket_fd, MAX_CLIENTS) == -1)) {
        perror("server listen failed");
        exit(1);
    }
}

bool parse_client_message (const char* message)
{
    if (strstr(message, BANNED_WORD) != NULL) {
        return true; 
    } else {
        return false;
    }
}

void handle_connections (int server_socket_fd)
{
    int       client_socket_fd;
    int       number_of_connections = 0;
    char*     client_message_buffer = malloc(CLIENT_MESSAGE_BUFFER_SIZE * sizeof(char));
    char*     client_ip_address = malloc(CLIENT_IP_BUFFER_SIZE * sizeof(char));
    ssize_t   bytes_read;
    socklen_t client_addrlen = sizeof(client_address);
    char*     server_message = "hello from server.";
    char*     file_content;
    char*     result_file_name;

    // handle connections
    for (int i = 0; i < MAX_CLIENTS; ++i ) {
        if ((client_socket_fd = accept(server_socket_fd, (struct sockaddr*)&client_address, &client_addrlen)) == -1) {
            perror("server accept client connection failed");
            exit(1);
        }

        inet_ntop(AF_INET, (struct in_addr*)&client_address.sin_addr, client_ip_address, CLIENT_IP_BUFFER_SIZE);
        printf("[+] server recived a connection from %s!\n", client_ip_address);
        number_of_connections++;

        // read
        if ((bytes_read = read(client_socket_fd, client_message_buffer, CLIENT_MESSAGE_BUFFER_SIZE)) == -1) {
            perror("server read error");
            exit(1);
        }

        client_message_buffer[strcspn(client_message_buffer, "\n")] = 0;
        printf("[<-] server read: %s [%zd]\n", client_message_buffer, bytes_read);

        /**
        * send
        */

        // banned word from client check 
        if (parse_client_message(client_message_buffer)) {
            printf("[!] client said the BANNED_WORD!\n");
            if (send(client_socket_fd, WARNING_SERVER_MESSAGE, strlen(WARNING_SERVER_MESSAGE), 0) == -1) {
                perror("server send error");
                exit(1);
            }
            continue;
            // blank message from client
        } else if (strcmp(client_message_buffer, "") == 0) {
            printf("[!] the client sent a blank message or crash\n");
            if (send(client_socket_fd, BLANK_MSG_FROM_CLIENT, strlen(BLANK_MSG_FROM_CLIENT), 0) == -1) {
                perror("server send error");
                exit(1);
            }
            // file request
        } else if (client_message_buffer[0] == 'F') {
            printf("[F] client request for a file\n");
            printf("[F] searching for the file...\n");

            if ((result_file_name = parse_and_check_for_file(client_message_buffer)) != NULL) {
                printf("[F] file found (%s), sending content\n", result_file_name);
                file_content = get_file_content(result_file_name);
                // send file content
                if (send(client_socket_fd, file_content, strlen(file_content), 0) == -1) {
                    perror("server send error");
                    exit(1);
                } 
            } else {
                printf("[F] file not found, terminating\n");
                if (send(client_socket_fd, "file not found.", strlen("file not found."), 0) == -1) {
                    perror("server send error");
                    exit(1);
                } 
            }
            // valid message from client
        } else {
            if (send(client_socket_fd, server_message, strlen(server_message), 0) == -1) {
                perror("server send error");
                exit(1);
            }
            printf("[->] server sended the message. [%s]\n", server_message);
        }
        close(client_socket_fd);
    }
    close(server_socket_fd);
    free(client_message_buffer);
    free(client_ip_address);
}

char* parse_and_check_for_file(const char* file_name) 
{
    char* buffer = malloc(1);
    bool found = false;
    int  j = 0;

    // remove 'F'
    for (int i = 1; i < strlen(file_name); i++) {
        buffer[j] = file_name[i];
        j++;
    }
    buffer[j] = '\0';

    // read the folder content
    DIR* folder;
    folder = opendir(SERVER_FOLDER_PATH);

    while((server_directory = readdir(folder)) != NULL) {
        if (strcmp(server_directory->d_name, buffer) == 0) {
            found = true;
        }
    }

    if (!found) {
        printf("[F] file: %s not found\n", buffer);
        return NULL;
    }

    closedir(folder);
    return buffer;
}

void append_char_to_str(char* str, char c)
{
    size_t len = strlen(str);
    str[len] = c;
    str[len + 1] = '\0';
}

char* get_file_content(char* file_name)
{
    char* path = malloc(strlen(SERVER_FOLDER_PATH) + strlen(file_name) + 1);
    // build the absolute path
    snprintf(path, strlen(SERVER_FOLDER_PATH) + strlen(file_name) + 1, "%s%s", SERVER_FOLDER_PATH, file_name); 

    char* content = malloc(3000);
    content[0] = '\0';

    // get content from the file
    FILE* file_;
    if ((file_ = fopen(path, "r")) == NULL) {
        perror("fopen fail");
        free(content);
        free(path);
        exit(1);
    }

    char c;
    while ((c = fgetc(file_)) != EOF) {
        append_char_to_str(content, c);
    }

    free(path);
    fclose(file_);
    return content;
}

