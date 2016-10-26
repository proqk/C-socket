#include <stdio.h>
#include <WinSock2.h>

#pragma comment(lib, "wsock32.lib")

int main(int argc, char *argv[]) {
	SOCKET socketdescriptor; //socket descriptor
	WSADATA WSAdata; //winsock data
	SOCKADDR_IN socketin; //socket struct

	char data[1024];
	FILE *fp;
	int ret, read;

	if (argc < 2) {
		printf("Write send file name. like 'Hello.txt', 'World.exe'\n");
	}

	char senddata[100];
	puts("data: ");
	fgets(senddata, sizeof(senddata), stdin);
	int datasize = sizeof(senddata);

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

	fp = fopen(argv[1], "rb");

	if (fp == NULL) {
		perror("file open error");
		closesocket(socketdescriptor);
		WSACleanup();
		return 0;
	}

	while (1) {
		read = fread(data, 1, 1024, fp);

		if (ferror(fp)) {
			perror("file read error");
			fclose(fp);
			closesocket(socketdescriptor);
			WSACleanup();
			return 0;
		}

		ret = send(socketdescriptor, data, read, 0);

		if (ret == SOCKET_ERROR || ret != read) {
			printf("data send failed: %u\n", WSAGetLastError());
			fclose(fp);
			closesocket(socketdescriptor);
			WSACleanup();
			return 0;
		}

		printf("You send data %d bite,\n", ret);
		if (feof(fp)) break;
	}

	fclose(fp);

	printf("You send %s file\n", argv[1]);

	shutdown(socketdescriptor, SD_BOTH);



	if (WSACleanup() != 0) {
		printf("WSACleanup failed, %u", WSAGetLastError());
		return 0;
	}
}