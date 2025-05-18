#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>

#define PORT 7000
#define BUFFER_SIZE 1024
#define ERROR -1

int main() {
    char message[BUFFER_SIZE];
    struct sockaddr_in server_addr;

    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == ERROR) {
        perror("Ошибка в socket");
        return EXIT_FAILURE;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;          
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(PORT);      

    while (1) {
        fputs("Enter message to send: ", stdout);
        if (fgets(message, BUFFER_SIZE, stdin) == NULL) break;  
        message[strcspn(message, "\n")] = '\0';

        if (!strcmp(message,"exit")) break;            

        int bytes_sended = sendto(sockfd, message, strlen(message), 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
        if (bytes_sended == ERROR) {
            perror("Ошибка в sendto");
            continue;
        }

        int bytes_received = recvfrom(sockfd, message, BUFFER_SIZE, 0, NULL, NULL);
        if (bytes_received == ERROR) {
            perror("Ошибка в recvfrom");
            continue;
        }

        message[bytes_received] = '\0';
        printf("Message from server: %s\n", message);        
    }

    close(sockfd);
    return EXIT_SUCCESS;;
}