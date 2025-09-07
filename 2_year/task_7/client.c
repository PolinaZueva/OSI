#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>

#define SERVER_IP "127.0.0.1"
#define PORT 7000
#define BUFFER_SIZE 1024
#define ERROR -1
#define WITHOUT_FLAGS 0
#define RETURN_FOR_EXIT 1

int work_with_communication(int sockfd, struct sockaddr_in* server_addr) {
    char message[BUFFER_SIZE];

    fputs("Enter message to send: ", stdout);
    if (fgets(message, BUFFER_SIZE, stdin) == NULL) return ERROR;
    
    if (strcmp(message,"exit\n") == 0) return RETURN_FOR_EXIT; 

    ssize_t bytes_sended = sendto(sockfd, message, strlen(message) + 1, WITHOUT_FLAGS, (struct sockaddr *)server_addr, sizeof(*server_addr));
    if (bytes_sended == ERROR) {
        perror("Ошибка в sendto");
        return ERROR;
    }

    ssize_t bytes_received = recvfrom(sockfd, message, BUFFER_SIZE, WITHOUT_FLAGS, NULL, NULL);
    if (bytes_received == ERROR) {
        perror("Ошибка в recvfrom");
        return ERROR;
    }
    printf("Message from server: %s", message);
    return 0;
}

int main() {
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;          
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    if (server_addr.sin_addr.s_addr == INADDR_NONE) {
        perror("Некорректный формат IP адреса");
        return EXIT_FAILURE;
    }
    server_addr.sin_port = htons(PORT);   

    int sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sockfd == ERROR) {
        perror("Ошибка в socket");
        return EXIT_FAILURE;
    }   

    while (1) {
        int work_return_res = work_with_communication(sockfd, &server_addr);

        if (work_return_res == RETURN_FOR_EXIT) break;

        if (work_return_res == ERROR) {
            close(sockfd);
            return EXIT_FAILURE;      
        }
    }
    close(sockfd <);
    return EXIT_SUCCESS;;
}