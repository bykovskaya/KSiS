#include<Windows.h>
#include<winsock2.h>
#include<Ws2tcpip.h>
#include<iphlpapi.h>
#include<stdio.h>
#include<stdlib.h>
#include<iostream>
#include<time.h>
using namespace std;
#pragma comment (lib, "Ws2_32.lib")

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define DEFAULT_PORT "27015"
#define DEFAULT_BUFLEN 10240

DWORD WINAPI WorkWithClient(LPVOID ClientSocket);

void createAns(char str[DEFAULT_BUFLEN]);

int  main(int argc, char* argv[])
{
	WSADATA wsaData;

	int iResult;
	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;

	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0)
	{
		cerr << "WSAStartup is failed: " << iResult;
		return 1;
	}

	struct addrinfo *result = NULL, *ptr = NULL, hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (iResult != 0)
	{
		cerr <<  "getaddrinfo failed: "<< iResult;
		WSACleanup();
		return 1;
	}

	SOCKET ListenSocket = INVALID_SOCKET;
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ListenSocket == INVALID_SOCKET)
	{
		cerr << "Error at socket(): "<< WSAGetLastError();
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}

	iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR)
	{
		cerr << "bind failed with error: "<< WSAGetLastError();
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}
	// free the memory allocated by the getaddrinfo function
	freeaddrinfo(result);
	if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR)
	{
		cerr<<"Listen failed with error: "<< WSAGetLastError();
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	SOCKET ClientSocket;
	sockaddr_in ClientAddr;
	int ClientAddrSIze = sizeof(ClientAddr);
	ClientSocket = INVALID_SOCKET;
	int ClientNum = 0;

	while (ClientSocket = accept(ListenSocket, (sockaddr *)&ClientAddr, &ClientAddrSIze))
	{

		if (ClientSocket == INVALID_SOCKET)
		{
			cerr <<"accept failed: "<< WSAGetLastError();
			closesocket(ListenSocket);
			WSACleanup();
			return 1;
		}
		
		HOSTENT *hst;
		hst = gethostbyaddr((char *)&ClientAddr.sin_addr, 4, AF_INET);
		printf("Client %s [%s] is accepted!\n", (hst)?hst->h_name:"", 
			inet_ntoa(ClientAddr.sin_addr));	//?
		
		ClientNum++;

		DWORD thID;
		CreateThread(NULL, NULL, WorkWithClient, &ClientSocket, NULL, &thID);

	}
	
	//closesocket(ClientSocket);
	WSACleanup();
	system("pause");
	return 0;
}

double ttime = 0.0;
DWORD WINAPI WorkWithClient(LPVOID ClientSocket)
{
	SOCKET sock;
	sock = ((SOCKET *)ClientSocket)[0];
	char buff[DEFAULT_BUFLEN];

	send(sock, "Hello! You are connected!\n", 28, 0);
	int iRes;
	while ((iRes = recv(sock, buff, DEFAULT_BUFLEN, 0)) && iRes != SOCKET_ERROR)
	{
		send(sock, buff, DEFAULT_BUFLEN, 0);
		cout << "\nCLient > " << buff << endl;
		int bufflen = strlen(buff);
		_itoa_s(bufflen, buff, DEFAULT_BUFLEN, 10);
		send(sock, buff, DEFAULT_BUFLEN, 0);
		send(sock, "", DEFAULT_BUFLEN, 0);
	}
		
	iRes = shutdown(sock, SD_SEND);
	if (iRes == SOCKET_ERROR) {
		cerr << "shutdown failed with error: " << WSAGetLastError();
		closesocket(sock);
	}
	return 0;
}
void createAns(char str[DEFAULT_BUFLEN])
{

}