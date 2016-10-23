#include <stdio.h>
#include <WinSock2.h>

#pragma comment(lib, "wsock32.lib")

int main() {
	SOCKET socketdescriptor; //socket descriptor
	WSADATA WSAdata; //winsock data
	SOCKADDR_IN socketin; //socket struct

	char data[100];
	fgets(data, sizeof(data), stdin);

	int datasize = sizeof(data);

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
	socketin.sin_addr.s_addr = inet_addr("127.0.0.1"); //I will access with this(all clinet) IP Address

	if (connect(socketdescriptor, (struct sockaddr*)&socketin, sizeof(socketin)) != 0) { //try to connect server
		printf("Can't connect, %u\n", WSAGetLastError());
		closesocket(socketdescriptor);
		WSACleanup();
		return 0;
	}

	if (send(socketdescriptor, data, datasize, 0) < datasize) { //data send
		printf("data send failed, %u\n", WSAGetLastError());
		closesocket(socketdescriptor);
		WSACleanup();
		return 0;
	}

	if (closesocket(socketdescriptor) != 0) {
		printf("remove socket failed, %d", WSAGetLastError()); //if socket can't finish
		WSACleanup();
		return 0;
	}

	data[strlen(data) - 1] = '\0';
	printf("You send data: %s\n", data);

	if (WSACleanup() != 0) {
		printf("WSACleanup failed, %u", WSAGetLastError());
		return 0;
	}
}