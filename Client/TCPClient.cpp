#include <iostream>
#include <WinSock2.h>
#include <MSWSock.h>
#include <WS2tcpip.h>
#include <thread>

#pragma comment(lib, "ws2_32.lib") // Winsock Library

using namespace std;

int main()
{
	WSAData wsaData;
	if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		return 1;
	}

	SOCKET clientSocket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (clientSocket == INVALID_SOCKET)
	{
		int errCode = ::WSAGetLastError();
		cout << "socket() failed with error code : " << errCode << endl;
		return 1;
	}

	SOCKADDR_IN serverAddr; // IPv4
	::memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	// serverAddr.sin_addr.s_addr = ::inet_addr("127.0.0.1");
	::inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);
	serverAddr.sin_port = ::htons(7777);

	// host to network short - Big-Endian

	if (::connect(clientSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
	{
		int errCode = ::WSAGetLastError();
		cout << "connect() failed with error code : " << errCode << endl;
		return 1;
	}

	// 연결 성공!
	cout << "Connected to server!" << endl;

	while (true)
	{
		char sendBuffer[100] = "Hello World!";

		int resultCode = ::send(clientSocket, sendBuffer, sizeof(sendBuffer), 0);

		if (resultCode == SOCKET_ERROR)
		{
			int errCode = ::WSAGetLastError();
			cout << "send() failed with error code : " << errCode << endl;
			return 1;
		}

		cout << "Sent " << resultCode << " bytes" << endl;

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


		::this_thread::sleep_for(1s);
	}

	// 소켓 리소스 반환
	::closesocket(clientSocket);

	::WSACleanup();

	return 0;
}