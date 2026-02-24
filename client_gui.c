// client_gui.c
#define _WIN32_WINNT 0x0601
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <process.h>
#include <stdio.h>

#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_PORT "27015"
#define DEFAULT_BUFLEN 512

SOCKET ConnectSocket = INVALID_SOCKET;
HWND hInput, hOutput;

void AppendText(HWND hEdit, const char* text) {
    int len = GetWindowTextLength(hEdit);
    SendMessage(hEdit, EM_SETSEL, (WPARAM)len, (LPARAM)len);
    SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)text);
}

unsigned __stdcall ReceiveThread(void* arg) {
    char recvbuf[DEFAULT_BUFLEN];
    int iResult;

    while (1) {
        ZeroMemory(recvbuf, DEFAULT_BUFLEN);
        iResult = recv(ConnectSocket, recvbuf, DEFAULT_BUFLEN, 0);
        if (iResult > 0) {
            strcat(recvbuf, "\r\n");
            AppendText(hOutput, recvbuf);
        } else {
            AppendText(hOutput, "Disconnected from server.\r\n");
            break;
        }
    }
    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    char buffer[DEFAULT_BUFLEN];

    switch (uMsg) {
        case WM_COMMAND:
            if (LOWORD(wParam) == 1) { // Send button
                GetWindowTextA(hInput, buffer, DEFAULT_BUFLEN);
                if (strlen(buffer) > 0) {
                    send(ConnectSocket, buffer, (int)strlen(buffer), 0);

                    // নিজের UI তে "Me:" হিসেবে দেখান
                    char selfMsg[DEFAULT_BUFLEN + 10];
                    sprintf(selfMsg, "Me: %s\r\n", buffer);
                    AppendText(hOutput, selfMsg);

                    SetWindowTextA(hInput, "");
                }
            }
            break;

        case WM_DESTROY:
            closesocket(ConnectSocket);
            WSACleanup();
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrev, LPSTR lpCmd, int nCmdShow) {
    WSADATA wsaData;
    struct sockaddr_in serverAddr;

    WSAStartup(MAKEWORD(2,2), &wsaData);

    ConnectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (ConnectSocket == INVALID_SOCKET) {
        MessageBox(NULL, "Socket creation failed", "Error", MB_OK);
        return 1;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(27015);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    // ipconfig------------192.168.0.199

    if (connect(ConnectSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        MessageBox(NULL, "Could not connect to server", "Error", MB_OK);
        return 1;
    }

    _beginthreadex(NULL, 0, ReceiveThread, NULL, 0, NULL);

    const char CLASS_NAME[] = "ChatClientWindow";
    WNDCLASS wc = { };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(0, CLASS_NAME, "Client UI (C)", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 400, 300, NULL, NULL, hInstance, NULL);

    hInput = CreateWindow("EDIT", "", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
        20, 20, 340, 25, hwnd, NULL, hInstance, NULL);

    hOutput = CreateWindow("EDIT", "", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY,
        20, 60, 340, 150, hwnd, NULL, hInstance, NULL);

    CreateWindow("BUTTON", "Send", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        150, 220, 80, 30, hwnd, (HMENU)1, hInstance, NULL);

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}


// gcc client_gui.c -o client_gui.exe -lws2_32 -mwindows
//  ./client_gui.exe 