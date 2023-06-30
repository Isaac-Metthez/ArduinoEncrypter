#include "../include/tcpServer.h"
#include "../include/crypto.h"
namespace Com {
    
    #define SERVER_PORT 20582
    #define SERVER_IP "192.168.169.155"
    #define AUTHENTICATION_HEADER "AuPK"
    #define DATA_HEADER "Data"
    #define CONFIG_HEADER "Conf"
    #define AGREEMENT_RENEWAL_HEADER "ReDH"
    #define PUBLIC_KEY_RENEWAL_HEADER "RePK"

    SOCKET TCPServer::serverSocket;
    SOCKET TCPServer::clientSocket;
    sockaddr_in TCPServer::serverAddress;
    sockaddr_in TCPServer::clientAddress;
    int TCPServer::requiredBytes = 160;

    void TCPServer::start() {
        Encryption::Crypto::loadAuthKeys();
        std::string msg = "AuPKfkpTASn74pj55DE1bw0vVof8IaEkEGTqIkaH3ovloko=DHPKWkS78GGKoSTRUB6/MOKcixwXDz9Xl0expuPZ5sES9C4=857e69edd592d585ab1b70bacb8e98e4f806c6eec5c23fd594f02d54102f312dfa0f748516a97d75246dacf9b7948036aab77c9f3420315aa1bbccaf46f38b0d";

  //      if (authenticate(msg)) {
  //          Encryption::Crypto::saveClientPublicKey();
  //          Encryption::Crypto::makeAgreement(msg.substr(52, 44));
  //      }
  //      else {
		//	return;
		//}

        WSInit();
        serverSocket = createSocket();
        serverAddress = bindSocket(serverSocket);
        listenOnSocket(serverSocket);
        acceptConnection(serverSocket);


        while (true) {
            msg = readFromSocket(clientSocket);
            if (msg == "") {
                break;
            }
            else if (!msg.rfind(AUTHENTICATION_HEADER, 0)) {
                if (authenticate(msg)) {
                    Encryption::Crypto::makeAgreement(msg.substr(52, 44));
                    sendToSocket(clientSocket, Encryption::Crypto::getAuthMessage());
                    requiredBytes = 44;
                }
                else {
                    break;
                }
            }
            else if (!msg.rfind(DATA_HEADER, 0)) {
                	std::cout << "Data" << std::endl;
            }
            else if (!msg.rfind(CONFIG_HEADER, 0)) {
                	std::cout << "Config" << std::endl;
            }
            else if (!msg.rfind(AGREEMENT_RENEWAL_HEADER, 0)) {
                	std::cout << "Agreement renewal" << std::endl;
            }
            else if (!msg.rfind(PUBLIC_KEY_RENEWAL_HEADER, 0)) {
                	std::cout << "Public key renewal" << std::endl;
            }
            else {
                const char* invalidHeaderError = "Invalid header\n";
                send(clientSocket, invalidHeaderError, static_cast<int>(strlen(invalidHeaderError)), 0);
            }
        }

        closesocket(clientSocket);
        closesocket(serverSocket);

        WSACleanup();
    }

    void TCPServer::WSInit() {
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            std::cerr << "Failed to initialize winsock" << std::endl;
            exit(1);
        }
    }

    SOCKET TCPServer::createSocket() {
        SOCKET s = socket(AF_INET, SOCK_STREAM, 0);
        if (s == INVALID_SOCKET) {
            std::cerr << "Failed to create socket" << std::endl;
            WSACleanup();
            exit(1);
        }
        return s;
    }

    sockaddr_in TCPServer::bindSocket(SOCKET& socket) {
        sockaddr_in address;
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(SERVER_PORT);

        if (bind(socket, reinterpret_cast<sockaddr*>(&address), sizeof(address)) == SOCKET_ERROR) {
            std::cerr << "Failed to bind socket" << std::endl;
            closesocket(socket);
            WSACleanup();
            exit(1);
        }
        return address;
    }

    void TCPServer::listenOnSocket(SOCKET& socket) {
        if (listen(socket, SOMAXCONN) == SOCKET_ERROR) {
            std::cerr << "Failed to listen on socket" << std::endl;
            closesocket(socket);
            WSACleanup();
            exit(1);
        }
        std::cout << "Server is listening on port " << SERVER_PORT << std::endl;
    }

    void TCPServer::acceptConnection(SOCKET serverSocket) {
        int clientAddressSize = sizeof(clientAddress);

        clientSocket = accept(serverSocket, reinterpret_cast<sockaddr*>(&clientAddress), &clientAddressSize);
        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "Failed to accept client connection" << std::endl;
            closesocket(serverSocket);
            WSACleanup();
            exit(1);
        }

        char clientAddressStr[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(clientAddress.sin_addr), clientAddressStr, INET_ADDRSTRLEN);
        std::cout << "Client connected: " << clientAddressStr << std::endl;
    }

    bool TCPServer::askForConfirmation() {
        while (true) {
            std::string input;
            std::cout << "Accept client authentication ? [Y/n]: ";
            std::getline(std::cin, input);

            if (input.empty() || input == "Y" || input == "y") {
                return true;
            }
            else if (input == "N" || input == "n") {
                return false;
            }
        }
    }

    bool TCPServer::sendToSocket(SOCKET socket, std::string authMessage) {
        int bytesSent = send(socket, authMessage.c_str(), static_cast<int>(authMessage.size()), 0);
        if (bytesSent == SOCKET_ERROR) {
            std::cerr << "Failed to send message: " << WSAGetLastError() << std::endl;
            return false;
        }
        std::cout << "Sent: " << authMessage << std::endl;
        return true;
    }

    std::string TCPServer::readFromSocket(SOCKET socket) {
        char buffer[1024];
        int bytesRead; 
        unsigned long availableBytes = 0;
        while (availableBytes < requiredBytes) {
			ioctlsocket(socket, FIONREAD, &availableBytes);
        }

        std::memset(buffer, 0, sizeof(buffer));
        bytesRead = recv(socket, buffer, requiredBytes, 0);
        if (bytesRead <= 0) {
            return "";
        }

        std::string str(buffer, std::strlen(buffer));
        std::cout << "Received: " << str << std::endl;
        return str;
    }

    bool TCPServer::authenticate(std::string authenticationMsg) {
        bool knownClient = Encryption::Crypto::checkForKnownKey(authenticationMsg.substr(4, 44));
        
        if (!Encryption::Crypto::verifySignature(authenticationMsg.substr(0, 96), authenticationMsg.substr(96, 64))) {
        //if (!Encryption::Crypto::verifySignature(authenticationMsg.substr(0, 96), authenticationMsg.substr(96, 128))) {
            return false;
        }
        else if (!knownClient) {
            if (askForConfirmation())
				Encryption::Crypto::saveClientPublicKey();
            else
				return false;
        }
        
        return true;
    }
}