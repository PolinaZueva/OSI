#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 7000
#define BUFFER_SIZE 1024
#define ERROR -1
#define WITHOUT_FLAGS 0

int work_with_client_request (int sockfd) {
    char message[BUFFER_SIZE];
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr); 

    ssize_t bytes_received = recvfrom(sockfd, message, BUFFER_SIZE, WITHOUT_FLAGS, (struct sockaddr *)&client_addr, &client_len);
    if (bytes_received == ERROR) {
        perror("Ошибка в recvfrom");
        return ERROR;
    }

    printf("Received from client: %s", message);

    ssize_t bytes_sended = sendto(sockfd, message, bytes_received, WITHOUT_FLAGS, (struct sockaddr *)&client_addr, client_len);
    if (bytes_sended == ERROR) {
        perror("Ошибка в sendto");
        return ERROR;
    }   
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
    server_addr.sin_port = htons(SERVER_PORT);         

    int sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sockfd == ERROR) {
        perror("Ошибка в socket");
        return EXIT_FAILURE;
    }  

    int bind_return_res = bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (bind_return_res == ERROR) {
        perror("Ошибка в bind");
        close(sockfd);
        return EXIT_FAILURE;
    }

    while (1) {
        int work_return_res = work_with_client_request(sockfd);
        if (work_return_res == ERROR) {
            close(sockfd);
            return EXIT_FAILURE;
        }
    }
    close(sockfd);
    return EXIT_SUCCESS;
}