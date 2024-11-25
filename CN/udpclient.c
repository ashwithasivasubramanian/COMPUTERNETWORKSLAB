#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib") // Winsock Library

int main() {
    WSADATA wsa;
    SOCKET s;
    struct sockaddr_in server;
    int slen = sizeof(server);
    char buffer[512];
    int num;

    // Initialize Winsock
    printf("Initializing Winsock...\n");
    if (WSAStartup(MAKEWORD(2,2),&wsa) != 0) {
        printf("Failed. Error Code : %d\n", WSAGetLastError());
        return 1;
    }
    printf("Winsock Initialized.\n");

    // Create socket
    if ((s = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET) {
        printf("Could not create socket : %d\n", WSAGetLastError());
        return 1;
    }
    printf("Socket created.\n");

    // Setup server address
    server.sin_family = AF_INET;
    server.sin_port = htons(8888);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Get number input from user
    printf("Enter a number: ");
    scanf("%d", &num);
    
    // Send number to server
    sprintf(buffer, "%d", num);
    if (sendto(s, buffer, strlen(buffer), 0, (struct sockaddr *) &server, slen) == SOCKET_ERROR) {
        printf("sendto() failed with error code : %d\n", WSAGetLastError());
        return 1;
    }

    // Receive result from server
    if (recvfrom(s, buffer, sizeof(buffer), 0, (struct sockaddr *) &server, &slen) == SOCKET_ERROR) {
        printf("recvfrom() failed with error code : %d\n", WSAGetLastError());
        return 1;
    }

    buffer[sizeof(buffer)] = '\0'; // Null-terminate the string
    printf("Factorial: %s\n", buffer);

    closesocket(s);
    WSACleanup();

    return 0;
}
