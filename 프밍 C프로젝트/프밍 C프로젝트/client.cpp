#include <stdio.h>
#include <WinSock2.h>
#include <conio.h>

#pragma comment(lib, "wsock32.lib")

int main(int argc, char *argv[]) {
	SOCKET socketdescriptor; //socket descriptor
	WSADATA WSAdata; //winsock data
	SOCKADDR_IN socketin; //socket struct
	int size, ret, timeout = 200;
	char buff[1000], data[1000], ID[100];


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

	puts("Start chatting client...");

	socketin.sin_family = AF_INET; //I will use TCP/IP or UDP protocol
	socketin.sin_port = htons(10000); //I will use FTP server port
	socketin.sin_addr.s_addr = inet_addr("127.0.0.1"); //I will access with this(all clinet) IP Address

	if (connect(socketdescriptor, (struct sockaddr*)&socketin, sizeof(socketin)) != 0) { //try to connect server
		printf("Can't connect, %u\n", WSAGetLastError());
		closesocket(socketdescriptor);
		WSACleanup();
		return 0;
	}

	size = sizeof(int);
	setsockopt(socketdescriptor, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, size);

	printf("Write your ID to use in the chatting room");
	fgets(ID, sizeof(ID), stdin);
	puts("Write the message and push the enter");

	while (1) {
		if (kbhit()) {
			fgets(buff, sizeof(buff), stdin);
			if (strcmp(buff, "END") == 0) {
				send(socketdescriptor, "END", 3, 0);
				break;
			}

			sprintf(data, "%s: %s", ID, buff);

			if (send(socketdescriptor, data, strlen(data), 0) < strlen(data)) {
				printf("send failed: %u\n", WSAGetLastError());
				closesocket(socketdescriptor); WSACleanup(); return 0;
			}
		}

		memset(data, 0, sizeof data);
		ret = recv(socketdescriptor, data, 1000, 0);

		if (ret == 0 || WSAGetLastError() == WSAETIMEDOUT) continue;

		if (ret == SOCKET_ERROR) {
			printf("recevie failed, %u\n", WSAGetLastError());
			closesocket(socketdescriptor); WSACleanup(); return 0;
		}

		puts(data);
	}


	if (closesocket(socketdescriptor) != 0) {
		printf("remove socket failed, %d", WSAGetLastError()); //if socket can't finish
		WSACleanup();
		return 0;
	}

	if (WSACleanup() != 0) {
		printf("WSACleanup failed, %u", WSAGetLastError());
		return 0;
	}
}