#include <iostream>
#include <WinSock2.h>
#include <MSWSock.h>
#include <WS2tcpip.h>
#include <string>

#pragma comment(lib, "ws2_32.lib") // Winsock Library

using namespace std;

int main(int argc, char* argv[]) // three arguments to be checked later
{
	string servName;	// server name
	int servPort;		// server port number
	char sendBuffer[1000];

	// Check and set arguments
	if (argc != 3) // 프로그램 이름도 포함됨
	{
		cout << "Error : three arguments are needed!" << endl;
		exit(1);
	}

	servName = argv[1];
	servPort = atoi(argv[2]);

	// Initialize Winsock
	WSAData wsaData;
	if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		return 1;
	}

	// Create Socket
	SOCKET clientSocket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (clientSocket == INVALID_SOCKET)
	{
		int errCode = ::WSAGetLastError();
		cout << "socket() failed with error code : " << errCode << endl;
		return 1;
	}

	// Cratee remote (server) socket address
	SOCKADDR_IN serverAddr;
	::memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	::inet_pton(AF_INET, servName.c_str(), &serverAddr.sin_addr);
	serverAddr.sin_port = ::htons((u_short)servPort);

	// Connect to the server
	if (::connect(clientSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
	{
		int errCode = ::WSAGetLastError();
		cout << "connect() failed with error code : " << errCode << endl;
		return 1;
	}

	// Send data to the server
	while (true)
	{
		cout << "Input message to send(q to quit) : ";
		cin.getline(sendBuffer, 1000);

		if (strcmp(sendBuffer, "q") == 0)
			break;

		int sendBytes = ::send(clientSocket, sendBuffer, (int)strlen(sendBuffer), 0);
		if (sendBytes == SOCKET_ERROR)
		{
			int errCode = ::WSAGetLastError();
			cout << "send() failed with error code : " << errCode << endl;
			return 1;
		}

		// Receive data from the server
		char recvBuffer[1000];
		int recvBytes = ::recv(clientSocket, recvBuffer, sizeof(recvBuffer), 0);

		if (recvBytes == SOCKET_ERROR)
		{
			int errCode = ::WSAGetLastError();
			cout << "recv() failed with error code : " << errCode << endl;
			return 1;
		}
		else if (recvBytes == 0)
		{
			cout << "Connection closed by peer" << endl;
			break;
		}
		else
		{
			recvBuffer[recvBytes] = '\0';
			cout << "Received message : " << recvBuffer << endl;
		}
	}



	// Close the socket
	::closesocket(clientSocket);

	// Finalize Winsock
	::WSACleanup();

	return 0;
}