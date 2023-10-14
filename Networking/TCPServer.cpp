#include <iostream>
#include <WinSock2.h>
#include <MSWSock.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib") // Winsock Library

using namespace std;

int main()
{
	// Initialize Winsock
	WSADATA wsaData;
	if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		cout << "WSAStartup() failed" << endl;
		return 1;
	}

	// ad : Address Family (AF_INET : IPv4, AF_INET6 : IPv6)
	// type : Socket Type (SOCK_STREAM : TCP, SOCK_DGRAM : UDP)
	// protocol : Protocol (IPPROTO_TCP : TCP, IPPROTO_UDP : UDP)
	// return : Socket Handle(descriptor)
	SOCKET listenSocket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (listenSocket == INVALID_SOCKET)
	{
		int errCode = ::WSAGetLastError();
		cout << "socket() failed with error code : " << errCode << endl;
		return 1;
	}
	
	SOCKADDR_IN serverAddr;
	::memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = ::htonl(INADDR_ANY); // 알아서 부탁합니다.
	serverAddr.sin_port = ::htons(7777);

	// 안내원 폰 개통! bind
	if (::bind(listenSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
	{
		int errCode = ::WSAGetLastError();
		cout << "bind() failed with error code : " << errCode << endl;
		return 1;
	}

	// 영업 시작!
	if (::listen(listenSocket, 10) == SOCKET_ERROR)	// 대기열 10개
	{
		int errCode = ::WSAGetLastError();
		cout << "listen() failed with error code : " << errCode << endl;
		return 1;
	}
	
	while (true)
	{
		SOCKADDR_IN clientAddr;
		::memset(&clientAddr, 0, sizeof(clientAddr));

		int clientAddrSize = sizeof(clientAddr);
		SOCKET clientSocket = ::accept(listenSocket, (SOCKADDR*)&clientAddr, &clientAddrSize);

		if (clientSocket == INVALID_SOCKET)
		{
			int errCode = ::WSAGetLastError();
			cout << "accept() failed with error code : " << errCode << endl;
			return 1;
		}

		// 손님 입장!
		char ipAddress[16];
		::inet_ntop(AF_INET, &clientAddr.sin_addr, ipAddress, sizeof(ipAddress));
		cout << "Client connected! IP Address : " << ipAddress << endl;

		while (true)
		{
			char recvBuffer[1000];

			int recvLen = ::recv(clientSocket, recvBuffer, sizeof(recvBuffer), 0);
			if (recvLen <= 0)
			{
				int errCode = ::WSAGetLastError();
				cout << "recv() failed with error code : " << errCode << endl;
				break;
			}

			cout << "Received " << recvLen << " bytes" << endl;
			cout << recvBuffer << endl;

			int resultCode = ::send(clientSocket, recvBuffer, recvLen, 0);

			if (resultCode == SOCKET_ERROR)
			{
				int errCode = ::WSAGetLastError();
				cout << "send() failed with error code : " << errCode << endl;
				return 1;
			}

		}

	}

	// 원속 종료
	::WSACleanup();

	return 0;
}