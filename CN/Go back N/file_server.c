// file_server.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    WSADATA wsaData;
    SOCKET sockfd, newsockfd;
    struct sockaddr_in server_addr, client_addr;
    int addr_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];
    FILE *file;
    int n;

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
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Bind socket
    if (bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("Bind failed. Error Code: %d\n", WSAGetLastError());
        closesocket(sockfd);
        WSACleanup();
        return 1;
    }

    // Listen for incoming connections
    if (listen(sockfd, 5) == SOCKET_ERROR) {
        printf("Listen failed. Error Code: %d\n", WSAGetLastError());
        closesocket(sockfd);
        WSACleanup();
        return 1;
    }

    printf("Server listening on port %d...\n", PORT);

    // Accept incoming connection
    newsockfd = accept(sockfd, (struct sockaddr*)&client_addr, &addr_len);
    if (newsockfd == INVALID_SOCKET) {
        printf("Accept failed. Error Code: %d\n", WSAGetLastError());
        closesocket(sockfd);
        WSACleanup();
        return 1;
    }

    printf("Connection established with client.\n");

    // Open file to save received data
    file = fopen("received_file.txt", "wb");
    if (file == NULL) {
        printf("File opening failed.\n");
        closesocket(newsockfd);
        closesocket(sockfd);
        WSACleanup();
        return 1;
    }

    // Receive file data with acknowledgment
    int packet_num = 0; // Track the packet number
    while ((n = recv(newsockfd, buffer, BUFFER_SIZE, 0)) > 0) {
        fwrite(buffer, 1, n, file);
        printf("Received packet %d\n", packet_num);

        // Send acknowledgment
        sprintf(buffer, "ACK %d", packet_num); // Acknowledge packet number
        send(newsockfd, buffer, strlen(buffer), 0);

        packet_num++; // Increment packet number
    }

    if (n == 0) {
        printf("File received successfully.\n");
    } else {
        printf("Error receiving file. Error Code: %d\n", WSAGetLastError());
    }

    // Cleanup
    fclose(file);
    closesocket(newsockfd);
    closesocket(sockfd);
    WSACleanup();
    return 0;
}
