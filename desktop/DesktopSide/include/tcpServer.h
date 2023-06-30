#include <iostream>
#include <regex>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma once
namespace Com {

	class TCPServer
	{
	private:
		static SOCKET serverSocket;
		static SOCKET clientSocket;
		static sockaddr_in serverAddress;
		static sockaddr_in clientAddress;
		static int requiredBytes;

		static void WSInit();
		static SOCKET createSocket();
		static sockaddr_in bindSocket(SOCKET& socket);
		static void listenOnSocket(SOCKET& socket);
		static void acceptConnection(SOCKET serverSocket);
		static bool askForConfirmation();
		static std::string readFromSocket(SOCKET socket);
		static bool authenticate(std::string authenticationStr);
		static bool sendToSocket(SOCKET socket, std::string authMessage);
	public:
		static void start();
	};
}