#include <stdio.h>
#include <WinSock2.h>
#include <winsock.h>

#pragma comment(lib, "wsock32.lib")

int main() {
	SOCKET socketdescriptor, csocketdescriptor; //socket descriptor
	WSADATA WSAdata; //winsock data
	struct sockaddr_in socketin; //socket struct
	struct sockaddr_in client_addr;
	int size = sizeof(client_addr);

	char data[1024];
	int ret;
	FILE *fp;
	int Timeout = 3000;

	char revdata[100];
	int datasize = sizeof(revdata);

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

	printf("Waiting to connect client access... \n");

	csocketdescriptor = accept(socketdescriptor, (struct sockaddr*)&client_addr, &size);

	if (csocketdescriptor == INVALID_SOCKET) {
		printf("access accept failed, %d", WSAGetLastError());
		closesocket(socketdescriptor);
		WSACleanup();
		return 0;
	}

	fp = fopen("c:\RecevieData.bin", "w+b");

	if (fp == NULL) {
		perror("file open error");
		closesocket(csocketdescriptor);
		closesocket(socketdescriptor);
		WSACleanup();
		return 0;
	}

	setsockopt(csocketdescriptor, SOL_SOCKET, SO_RCVTIMEO, (char*)&Timeout, size);

	while (1) {
		ret = recv(csocketdescriptor, data, 1024, 0);

		if (ret == 0) break;
		if (ret == SOCKET_ERROR) {
			printf("receive error: %u\n", WSAGetLastError());
			closesocket(csocketdescriptor);
			closesocket(socketdescriptor);
			WSACleanup();
			fclose(fp);
			return 0;
		}
	}

	fwrite(data, 1, ret, fp);
	printf("You receive %d\n", ret);

	fclose(fp);
	shutdown(socketdescriptor, SD_BOTH);



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