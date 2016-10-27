#include <stdio.h>
#include <WinSock2.h>
#include <conio.h>

#pragma comment(lib, "wsock32.lib")

int main() {
	SOCKET socketdescriptor, csocketdescriptor; //socket descriptor
	WSADATA WSAdata; //winsock data
	struct sockaddr_in socketin; //socket struct
	struct sockaddr_in client_addr;
	int size = sizeof(client_addr);
	char buff[1000], data[1000], ID[100];
	int timeout = 200, ret;

	if (WSAStartup(WINSOCK_VERSION, &WSAdata) != 0) { //winsock version check
		printf("WSAStartup failed, %d\n", WSAGetLastError());
		return 0;
	}

	socketdescriptor = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); //create TCP/IP socket

	if (socketdescriptor == INVALID_SOCKET) {
		printf("Create socket failed, %d\n", WSAGetLastError()); //if socket() get error
		WSACleanup(); //finish WS2_32.DLL
		return 0;
	}

	socketin.sin_family = AF_INET; //I will use TCP/IP or UDP protocol
	socketin.sin_port = htons(10000); //I will use FTP server port
	socketin.sin_addr.s_addr = htonl(ADDR_ANY); //I will access with this(all clinet) IP Address

	if (bind(socketdescriptor, (struct sockaddr*)&socketin, sizeof(socketin)) == SOCKET_ERROR) {
		printf("bind failed, %d\n", WSAGetLastError());
		closesocket(socketdescriptor);
		WSACleanup();
		return 0;
	}

	if (listen(socketdescriptor, SOMAXCONN) != 0) {
		printf("listen mode failed, %d", WSAGetLastError());
		closesocket(socketdescriptor);
		WSACleanup();
		return 0;
	}

	printf("Start Chatting server...\n");

	csocketdescriptor = accept(socketdescriptor, (struct sockaddr*)&client_addr, &size);

	if (csocketdescriptor == INVALID_SOCKET) {
		printf("access accept failed, %d", WSAGetLastError());
		closesocket(socketdescriptor);
		WSACleanup();
		return 0;
	}

	printf("Write your ID to use in the chatting room");
	fgets(ID, sizeof(ID), stdin);
	puts("Write the message and push the enter");

	size = sizeof(int);

	setsockopt(csocketdescriptor, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, size);

	while (1) {
		if (kbhit()) {
			fgets(buff, sizeof(buff), stdin);
			if (strcmp(buff, "END") == 0) {
				send(socketdescriptor, "END", 3, 0);
				break;
			}

			sprintf(data, "%s: %s", ID, buff);

			if (send(csocketdescriptor, data, strlen(data), 0) < strlen(data)) {
				printf("send failed: %u\n", WSAGetLastError());
				closesocket(socketdescriptor); closesocket(csocketdescriptor); WSACleanup(); return 0;
			}
		}

		memset(data, 0, sizeof data);
		ret = recv(csocketdescriptor, data, 1000, 0);

		if (ret == 0 || WSAGetLastError() == WSAETIMEDOUT) continue;

		if (ret == SOCKET_ERROR) {
			printf("recevie failed, %u\n", WSAGetLastError());
			closesocket(socketdescriptor); closesocket(csocketdescriptor); WSACleanup(); return 0;
		}

		puts(data);
	}

	if (closesocket(csocketdescriptor) != 0 || closesocket(socketdescriptor) != 0) {
		printf("Remove socket failed, %d", WSAGetLastError());
		WSACleanup();
		return 0;
	}

	if (WSACleanup() != 0) {
		printf("WSACleanup failed, %u", WSAGetLastError());
		return 0;
	}
}