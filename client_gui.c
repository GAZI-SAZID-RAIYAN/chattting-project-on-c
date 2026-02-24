// client_gui.c
// Windows GUI Chat Client (Improved Programming Style)

#define _WIN32_WINNT 0x0601
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <process.h>
#include <stdio.h>
#include <string.h>

#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_PORT        27015
#define DEFAULT_BUFLEN      512
#define WINDOW_WIDTH        400
#define WINDOW_HEIGHT       300

/* ===================== GLOBAL VARIABLES ===================== */

SOCKET g_connect_socket = INVALID_SOCKET;
HWND g_input_box;
HWND g_output_box;

/* ===================== FUNCTION DECLARATIONS ===================== */

int initialize_network(void);
int connect_to_server(const char* ip_address);
void create_main_window(HINSTANCE h_instance, int n_cmd_show);
void append_text(HWND edit_control, const char* text);
unsigned __stdcall receive_thread(void* arg);

/* ===================== FUNCTION DEFINITIONS ===================== */

/* Initializes Winsock */
int initialize_network(void)
{
    WSADATA wsa_data;
    return (WSAStartup(MAKEWORD(2, 2),
                       &wsa_data) == 0);
}

/* Connects client to server */
int connect_to_server(const char* ip_address)
{
    struct sockaddr_in server_addr;

    g_connect_socket =
        socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (g_connect_socket == INVALID_SOCKET)
        return 0;

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(DEFAULT_PORT);
    server_addr.sin_addr.s_addr =
        inet_addr(ip_address);

    return (connect(g_connect_socket,
           (struct sockaddr*)&server_addr,
           sizeof(server_addr)) != SOCKET_ERROR);
}

/* Appends text to output edit control */
void append_text(HWND edit_control,
                 const char* text)
{
    int length =
        GetWindowTextLength(edit_control);

    SendMessage(edit_control,
                EM_SETSEL,
                (WPARAM)length,
                (LPARAM)length);

    SendMessage(edit_control,
                EM_REPLACESEL,
                0,
                (LPARAM)text);
}

/* Thread for receiving messages */
unsigned __stdcall receive_thread(void* arg)
{
    char recv_buffer[DEFAULT_BUFLEN];
    int result;

    while (1) {
        ZeroMemory(recv_buffer,
                   DEFAULT_BUFLEN);

        result = recv(g_connect_socket,
                      recv_buffer,
                      DEFAULT_BUFLEN,
                      0);

        if (result > 0) {
            strncat(recv_buffer,
                    "\r\n",
                    DEFAULT_BUFLEN - strlen(recv_buffer) - 1);

            append_text(g_output_box,
                        recv_buffer);
        }
        else {
            append_text(g_output_box,
                        "Disconnected from server.\r\n");
            break;
        }
    }

    return 0;
}

/* Window procedure for GUI events */
LRESULT CALLBACK window_proc(HWND hwnd,
                             UINT message,
                             WPARAM w_param,
                             LPARAM l_param)
{
    char buffer[DEFAULT_BUFLEN];

    switch (message) {

        case WM_COMMAND:
            if (LOWORD(w_param) == 1) {

                GetWindowTextA(g_input_box,
                               buffer,
                               DEFAULT_BUFLEN);

                if (strlen(buffer) > 0) {

                    send(g_connect_socket,
                         buffer,
                         (int)strlen(buffer),
                         0);

                    char self_msg[DEFAULT_BUFLEN + 10];

                    snprintf(self_msg,
                             sizeof(self_msg),
                             "Me: %s\r\n",
                             buffer);

                    append_text(g_output_box,
                                self_msg);

                    SetWindowTextA(g_input_box,
                                   "");
                }
            }
            break;

        case WM_DESTROY:
            closesocket(g_connect_socket);
            WSACleanup();
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProc(hwnd,
                         message,
                         w_param,
                         l_param);
}

/* Creates main GUI window */
void create_main_window(HINSTANCE h_instance,
                        int n_cmd_show)
{
    const char class_name[] = "ChatClientWindow";

    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = window_proc;
    wc.hInstance = h_instance;
    wc.lpszClassName = class_name;

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0,
        class_name,
        "Client UI (C)",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        NULL,
        NULL,
        h_instance,
        NULL
    );

    g_input_box = CreateWindow(
        "EDIT", "",
        WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
        20, 20, 340, 25,
        hwnd, NULL,
        h_instance, NULL
    );

    g_output_box = CreateWindow(
        "EDIT", "",
        WS_CHILD | WS_VISIBLE | WS_BORDER |
        ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY,
        20, 60, 340, 150,
        hwnd, NULL,
        h_instance, NULL
    );

    CreateWindow(
        "BUTTON", "Send",
        WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        150, 220, 80, 30,
        hwnd, (HMENU)1,
        h_instance, NULL
    );

    ShowWindow(hwnd, n_cmd_show);
    UpdateWindow(hwnd);
}

/* ===================== WinMain ===================== */

int WINAPI WinMain(HINSTANCE h_instance,
                   HINSTANCE h_prev,
                   LPSTR lp_cmd,
                   int n_cmd_show)
{
    if (!initialize_network()) {
        MessageBox(NULL,
                   "Network initialization failed",
                   "Error",
                   MB_OK);
        return 1;
    }

    if (!connect_to_server("127.0.0.1")) {
        MessageBox(NULL,
                   "Could not connect to server",
                   "Error",
                   MB_OK);
        return 1;
    }

    _beginthreadex(NULL,
                   0,
                   receive_thread,
                   NULL,
                   0,
                   NULL);

    create_main_window(h_instance,
                       n_cmd_show);

    MSG msg = { 0 };

    while (GetMessage(&msg,
                      NULL,
                      0,
                      0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}