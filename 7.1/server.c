#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <string.h>

#define PORT 7000
#define BUFFER_SIZE 1024
#define ERROR -1

int main() {
    char message[BUFFER_SIZE];
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == ERROR) {
        perror("Ошибка в socket");
        return EXIT_FAILURE;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;          
    server_addr.sin_addr.s_addr = INADDR_ANY;  //принимаем соединения с любого IP
    server_addr.sin_port = htons(PORT);       

    int bind_res = bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (bind_res == ERROR) {
        perror("Ошибка в bind");
        close(sockfd);
        return EXIT_FAILURE;
    }

    while (1) {
        int bytes_received = recvfrom(sockfd, message, BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, &client_len);
        if (bytes_received == ERROR) {
            perror("Ошибка в recvfrom");
            continue;
        }

        message[bytes_received] = '\0';
        printf("Received from client: %s\n", message);

        int bytes_sended = sendto(sockfd, message, bytes_received, 0, (struct sockaddr *)&client_addr, client_len);
        if (bytes_sended == ERROR) {
            perror("Ошибка в sendto");
            continue;
        }
    }

    close(sockfd);
    return EXIT_SUCCESS;
}