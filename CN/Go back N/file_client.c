// file_client.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT 8080
#define BUFFER_SIZE 1024

void send_file(FILE *file, SOCKET sockfd) {
    char buffer[BUFFER_SIZE];
    int n, ack_len;
    char ack_buffer[BUFFER_SIZE];

    int packet_num = 0; // Track the packet number

    while ((n = fread(buffer, 1, BUFFER_SIZE, file)) > 0) {
        send(sockfd, buffer, n, 0);
        printf("Sent packet %d\n", packet_num);

        // Wait for acknowledgment from the server
        ack_len = recv(sockfd, ack_buffer, BUFFER_SIZE, 0);
        if (ack_len > 0) {
            ack_buffer[ack_len] = '\0'; // Null terminate the acknowledgment message
            printf("Received acknowledgment: %s\n", ack_buffer);
        } else {
            printf("Failed to receive acknowledgment.\n");
            break;
        }

        packet_num++; // Increment packet number
    }
}

int main() {
    WSADATA wsaData;
    SOCKET sockfd;
    struct sockaddr_in server_addr;
    FILE *file;
    char *filename = "kec.txt";

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup failed. Error Code: %d\n", WSAGetLastError());
        return 1;
    }

    // Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == INVALID_SOCKET) {
        printf("Socket creation failed. Error Code: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    // Server address setup
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Connect to server
    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("Connection failed. Error Code: %d\n", WSAGetLastError());
        closesocket(sockfd);
        WSACleanup();
        return 1;
    }

    printf("Connected to server.\n");

    // Open file to send
    file = fopen(filename, "rb");
    if (file == NULL) {
        printf("File not found: %s\n", filename);
        closesocket(sockfd);
        WSACleanup();
        return 1;
    }

    // Send the file
    send_file(file, sockfd);
    printf("File sent successfully.\n");

    // Cleanup
    fclose(file);
    closesocket(sockfd);
    WSACleanup();
    return 0;
}
