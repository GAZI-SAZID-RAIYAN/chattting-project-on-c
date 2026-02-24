// server.c
#define _WIN32_WINNT 0x0601
#include <winsock2.h>
#include <ws2tcpip.h>
#include <process.h>
#include <stdio.h>
#include <stdlib.h>

#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_PORT "27015"
#define DEFAULT_BUFLEN 512
#define MAX_CLIENTS 10

typedef struct {
    SOCKET socket;
    int id;
} CLIENT;

CLIENT* clients[MAX_CLIENTS];
int clientCount = 0;

void broadcast(const char* message, int senderId) {
    for (int i = 0; i < clientCount; ++i) {
       
        send(clients[i]->socket, message, (int)strlen(message), 0);
    }
}

unsigned int __stdcall ClientThread(void* arg) {
    CLIENT* client = (CLIENT*)arg;
    char recvbuf[DEFAULT_BUFLEN];
    int iResult;

    printf("Client %d connected.\n", client->id);

    while (1) {
        ZeroMemory(recvbuf, DEFAULT_BUFLEN);
        iResult = recv(client->socket, recvbuf, DEFAULT_BUFLEN, 0);
        if (iResult > 0) {
            printf("Client %d: %s\n", client->id, recvbuf);

            char msgWithId[DEFAULT_BUFLEN + 50];
            sprintf(msgWithId, "Client %d: %s", client->id, recvbuf);

            broadcast(msgWithId, client->id);
        } else {
            printf("Client %d disconnected.\n", client->id);
            break;
        }
    }

    closesocket(client->socket);

    // Remove client
    for (int i = 0; i < clientCount; i++) {
        if (clients[i]->id == client->id) {
            for (int j = i; j < clientCount - 1; j++)
                clients[j] = clients[j + 1];
            clientCount--;
            break;
        }
    }

    free(client);
    return 0;
}

int main() {
    WSADATA wsaData;
    SOCKET ListenSocket = INVALID_SOCKET;
    struct addrinfo *result = NULL, hints;

    WSAStartup(MAKEWORD(2,2), &wsaData);

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    listen(ListenSocket, SOMAXCONN);

    printf("Server waiting for clients...\n");

    while (1) {
        SOCKET ClientSocket = accept(ListenSocket, NULL, NULL);
        if (ClientSocket != INVALID_SOCKET) {
            CLIENT* client = (CLIENT*)malloc(sizeof(CLIENT));
            client->socket = ClientSocket;
            client->id = clientCount + 1;
            clients[clientCount++] = client;
            _beginthreadex(NULL, 0, ClientThread, client, 0, NULL);
        }
    }

    closesocket(ListenSocket);
    WSACleanup();
    return 0;
}


// ./server.exe
 
// gcc server.c -o server.exe -lws2_32
















