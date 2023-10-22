#include <iostream>
#include <WinSock2.h>
#include <MSWSock.h>
#include <WS2tcpip.h>
#include <string>

#pragma comment(lib, "ws2_32.lib") // Winsock Library

#define SERV_PORT 7777

using namespace std;

int main()
{
	int waitSize = 16;

	// Initialize Winsock
	WSADATA wsaData;
	if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		cout << "WSAStartup() failed" << endl;
		return 1;
	}

	// Create listen socket
	SOCKET listenSocket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (listenSocket == INVALID_SOCKET)
	{
		int errCode = ::WSAGetLastError();
		cout << "socket() failed with error code : " << errCode << endl;
		::WSACleanup();
		return 1;
	}

	// Create local (server) socket address
	SOCKADDR_IN serverAddr;
	::memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = ::htonl(INADDR_ANY); // Default IP Address
	serverAddr.sin_port = ::htons(SERV_PORT); // Default port number

	// Bind listen socket to the local socket address
	if (::bind(listenSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
	{
		int errCode = ::WSAGetLastError();
		cout << "bind() failed with error code : " << errCode << endl;
		::closesocket(listenSocket);
		::WSACleanup();
		exit(1);
	}

	// Listen to the client
	if (::listen(listenSocket, waitSize) == SOCKET_ERROR)
	{
		int errCode = ::WSAGetLastError();
		cout << "listen() failed with error code : " << errCode << endl;
		::closesocket(listenSocket);
		::WSACleanup();
		exit(1);
	}

	// Handle the connection
	while (true) // Run forever
	{
		// Wait for a client to connect
		SOCKADDR_IN clientAddr;
		::memset(&clientAddr, 0, sizeof(clientAddr));

		int clientAddrSize = sizeof(clientAddr);

		// Accept connections from client
		SOCKET clientSocket = ::accept(listenSocket, (SOCKADDR*)&clientAddr, &clientAddrSize);
		if (clientSocket == INVALID_SOCKET)
		{
			int errCode = ::WSAGetLastError();
			cout << "accept() failed with error code : " << errCode << endl;
			return 1;
		}

		// Data transfer section
		while (true)
		{
			char recvBuffer[256] = { 0, };

			// Receive data from the client
			int recvLen = ::recv(clientSocket, recvBuffer, sizeof(recvBuffer), 0);

			if (recvLen <= 0)
			{
				int errCode = ::WSAGetLastError();
				cout << "recv() failed with error code : " << errCode << endl;
				break;
			}

			// Process the received data
			recvBuffer[recvLen] = '\0';
			cout << "Received message from client : " << recvBuffer << endl;

			// Respond to the client
			string response = "You sent ";
			response += to_string(recvLen);
			response += " bytes";

			const char* response_c = response.c_str();

			// send back
			int resultCode = ::send(clientSocket, response_c, (int)strlen(response_c), 0);

			if (resultCode == SOCKET_ERROR)
			{
				int errCode = ::WSAGetLastError();
				cout << "send() failed with error code : " << errCode << endl;
				return 1;
			}
		}
	} // End of loop

	// Finalize Winsock
	::WSACleanup();

	return 0;
}