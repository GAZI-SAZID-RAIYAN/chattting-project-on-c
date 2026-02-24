// server.c
// Multi-client chat server using Winsock (Improved Programming Style)

#define _WIN32_WINNT 0x0601
#include <winsock2.h>
#include <ws2tcpip.h>
#include <process.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_PORT        "27015"
#define DEFAULT_BUFLEN      512
#define MAX_CLIENTS         10

typedef struct {
    SOCKET socket;
    int id;
} client_t;

/* ===================== GLOBAL VARIABLES ===================== */

client_t* g_clients[MAX_CLIENTS];
int g_client_count = 0;

/* ===================== FUNCTION DECLARATIONS ===================== */

int initialize_winsock(void);
SOCKET create_listening_socket(void);
void accept_clients(SOCKET listen_socket);
void broadcast_message(const char* message);
unsigned __stdcall client_thread(void* arg);
void remove_client(int client_id);

/* ===================== FUNCTION DEFINITIONS ===================== */

/* Initializes Winsock library */
int initialize_winsock(void)
{
    WSADATA wsa_data;
    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
        printf("WSAStartup failed.\n");
        return 0;
    }
    return 1;
}

/* Creates, binds and listens on server socket */
SOCKET create_listening_socket(void)
{
    struct addrinfo hints, *result = NULL;
    SOCKET listen_socket = INVALID_SOCKET;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    if (getaddrinfo(NULL, DEFAULT_PORT, &hints, &result) != 0) {
        printf("getaddrinfo failed.\n");
        return INVALID_SOCKET;
    }

    listen_socket = socket(result->ai_family,
                           result->ai_socktype,
                           result->ai_protocol);

    if (listen_socket == INVALID_SOCKET) {
        printf("Socket creation failed.\n");
        freeaddrinfo(result);
        return INVALID_SOCKET;
    }

    if (bind(listen_socket, result->ai_addr,
             (int)result->ai_addrlen) == SOCKET_ERROR) {
        printf("Bind failed.\n");
        freeaddrinfo(result);
        closesocket(listen_socket);
        return INVALID_SOCKET;
    }

    freeaddrinfo(result);

    if (listen(listen_socket, SOMAXCONN) == SOCKET_ERROR) {
        printf("Listen failed.\n");
        closesocket(listen_socket);
        return INVALID_SOCKET;
    }

    return listen_socket;
}

/* Sends message to all connected clients */
void broadcast_message(const char* message)
{
    for (int i = 0; i < g_client_count; ++i) {
        send(g_clients[i]->socket,
             message,
             (int)strlen(message),
             0);
    }
}

/* Removes disconnected client from global list */
void remove_client(int client_id)
{
    for (int i = 0; i < g_client_count; ++i) {
        if (g_clients[i]->id == client_id) {
            for (int j = i; j < g_client_count - 1; ++j) {
                g_clients[j] = g_clients[j + 1];
            }
            g_client_count--;
            break;
        }
    }
}

/* Thread function to handle a connected client */
unsigned __stdcall client_thread(void* arg)
{
    client_t* client = (client_t*)arg;
    char recv_buffer[DEFAULT_BUFLEN];
    int result;

    printf("Client %d connected.\n", client->id);

    while (1) {
        ZeroMemory(recv_buffer, DEFAULT_BUFLEN);

        result = recv(client->socket,
                      recv_buffer,
                      DEFAULT_BUFLEN,
                      0);

        if (result > 0) {
            printf("Client %d: %s\n",
                   client->id,
                   recv_buffer);

            char message[DEFAULT_BUFLEN + 50];

            snprintf(message,
                     sizeof(message),
                     "Client %d: %s",
                     client->id,
                     recv_buffer);

            broadcast_message(message);
        }
        else {
            printf("Client %d disconnected.\n",
                   client->id);
            break;
        }
    }

    closesocket(client->socket);
    remove_client(client->id);
    free(client);

    return 0;
}

/* Accepts incoming client connections */
void accept_clients(SOCKET listen_socket)
{
    while (1) {
        SOCKET client_socket =
            accept(listen_socket, NULL, NULL);

        if (client_socket != INVALID_SOCKET &&
            g_client_count < MAX_CLIENTS) {

            client_t* new_client =
                (client_t*)malloc(sizeof(client_t));

            new_client->socket = client_socket;
            new_client->id = g_client_count + 1;

            g_clients[g_client_count++] = new_client;

            _beginthreadex(NULL,
                           0,
                           client_thread,
                           new_client,
                           0,
                           NULL);
        }
    }
}

/* ===================== MAIN FUNCTION ===================== */

int main(void)
{
    if (!initialize_winsock())
        return 1;

    SOCKET listen_socket =
        create_listening_socket();

    if (listen_socket == INVALID_SOCKET)
        return 1;

    printf("Server waiting for clients...\n");

    accept_clients(listen_socket);

    closesocket(listen_socket);
    WSACleanup();

    return 0;
}