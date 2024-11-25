#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib") // Winsock Library

// Function to calculate factorial
unsigned long long factorial(int n) {
    if (n == 0 || n == 1) {
        return 1;
    }
    unsigned long long result = 1;
    for (int i = 2; i <= n; i++) {
        result *= i;
    }
    return result;
}

int main() {
    WSADATA wsa;
    SOCKET s;
    struct sockaddr_in server, client;
    int slen, recv_len;
    char buffer[512];
    int num;

    slen = sizeof(client);

    // Initialize Winsock
    printf("Initializing Winsock...\n");
    if (WSAStartup(MAKEWORD(2,2),&wsa) != 0) {
        printf("Failed. Error Code : %d\n", WSAGetLastError());
        exit(EXIT_FAILURE);
    }
    printf("Winsock Initialized.\n");

    // Create socket
    if ((s = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET) {
        printf("Could not create socket : %d\n", WSAGetLastError());
        exit(EXIT_FAILURE);
    }
    printf("Socket created.\n");

    // Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(8000);

    // Bind
    if (bind(s, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR) {
        printf("Bind failed with error code : %d\n", WSAGetLastError());
        exit(EXIT_FAILURE);
    }
    printf("Bind done.\n");

    // Listen for incoming data
    while(1) {
        printf("Waiting for data...\n");
        fflush(stdout);

        // Receive a message from client
        if ((recv_len = recvfrom(s, buffer, sizeof(buffer), 0, (struct sockaddr *)&client, &slen)) == SOCKET_ERROR) {
            printf("recvfrom() failed with error code : %d\n", WSAGetLastError());
            exit(EXIT_FAILURE);
        }

        buffer[recv_len] = '\0'; // Null-terminate the string
        num = atoi(buffer); // Convert string to integer

        printf("Received number: %d\n", num);

        // Calculate factorial
        unsigned long long fact = factorial(num);

        // Send factorial result back to client
        sprintf(buffer, "%llu", fact);
        if (sendto(s, buffer, strlen(buffer), 0, (struct sockaddr *)&client, slen) == SOCKET_ERROR) {
            printf("sendto() failed with error code : %d\n", WSAGetLastError());
            exit(EXIT_FAILURE);
        }
    }

    closesocket(s);
    WSACleanup();

    return 0;
}
