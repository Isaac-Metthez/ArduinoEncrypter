#include "../include/server.h"
#include <chrono>
namespace com {
    bool Server::start() {
        security = ComSecurityLevel::SIMPLE;
        sendOutputs = true;
        
        
        bool result = false;
        crypto = new encryption::Crypto();
        inputBoolPayloadSize = 16 * (digitalInputs.size() % BIT_PER_BLOCK == 0 ? digitalInputs.size() / BIT_PER_BLOCK : digitalInputs.size() / BIT_PER_BLOCK + 1);
        inputIntPayloadSize= 16 * (analogInputs.size() % INT_PER_BLOCK == 0 ? analogInputs.size() / INT_PER_BLOCK : analogInputs.size() / INT_PER_BLOCK + 1);
        inputPayloadSize = inputBoolPayloadSize + inputIntPayloadSize;
        outputBoolPayloadSize = 16 * (digitalOutputs.size() % BIT_PER_BLOCK == 0 ? digitalOutputs.size() / BIT_PER_BLOCK : digitalOutputs.size() / BIT_PER_BLOCK + 1);
        outputIntPayloadSize= 16 * (analogOutputs.size() % INT_PER_BLOCK == 0 ? analogOutputs.size() / INT_PER_BLOCK : analogOutputs.size() / INT_PER_BLOCK + 1);
        outputPayloadSize = outputBoolPayloadSize + outputIntPayloadSize;
        ++clientSequence;
        ++serverSequence;

        switch (security) {
            case ComSecurityLevel::SIMPLE:
                requiredBytes = DATA_HEADER_LENGTH + inputBoolPayloadSize;
                break;
            case ComSecurityLevel::ENCRYPTED:
            case ComSecurityLevel::SECURE:
                requiredBytes = AUTH_MESSAGE_LENGTH;
                break;
        }

        char msg[1024];
        
        WSInit();
        serverSocket = createSocket();
        serverAddress = bindSocket(serverSocket);
        listenOnSocket(serverSocket);
        acceptConnection(serverSocket);
        auto startTime = std::chrono::high_resolution_clock::now();
        int n = 0;
        while (true) {
            if (sendOutputs) {
                sendData();
                //sendOutputs = false;
            }
            if (!readFromSocket(clientSocket, msg)) {
                break;
            }
            if (std::strncmp(msg, encryption::AUTHENTICATION_HEADER, std::strlen(encryption::AUTHENTICATION_HEADER)) == 0) {
                if (authenticate(msg)) {
                    crypto->makeAgreement(std::string(msg + std::strlen(encryption::AUTHENTICATION_HEADER) + encryption::B64_KEY_LENGTH + std::strlen(encryption::AGREEMENT_HEADER), encryption::B64_KEY_LENGTH));
                    std::string authMessage = crypto->getAuthMessage();
                    sendToSocket(clientSocket, authMessage.c_str(), authMessage.size());
                    requiredBytes = DATA_HEADER_LENGTH + inputPayloadSize + (security == ComSecurityLevel::SECURE ? encryption::SIGNATURE_LENGTH : 0);
                    sendOutputs = true;
                }
                else {
                    break;
                }
            }
            else {
                if (security != ComSecurityLevel::SECURE || crypto->verifySignature(msg, msg + DATA_HEADER_LENGTH + inputPayloadSize - encryption::SIGNATURE_LENGTH)) {
                    if(security >= ComSecurityLevel::SIMPLE)
                        crypto->decrypt(msg, DATA_HEADER_LENGTH + inputPayloadSize);
                    if (std::strncmp(msg, encryption::DATA_HEADER, std::strlen(encryption::DATA_HEADER)) == 0) {
                        if (verifySequence(msg + std::strlen(encryption::DATA_HEADER))) {
                            parseInput(msg + DATA_HEADER_LENGTH);
                        }
                        n++;
                        if (n == 1000)
                            break;
                    }
                    else {
                        const char* invalidHeaderError = "Invalid header\n";
                        send(clientSocket, invalidHeaderError, static_cast<int>(strlen(invalidHeaderError)), 0);
                        break;
                    }
                ++clientSequence;
                }
                else {
                    const char* invalidSignatureError = "Invalid signature\n";
                    send(clientSocket, invalidSignatureError, static_cast<int>(strlen(invalidSignatureError)), 0);
                    break;
                }
            }
        }
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
        std::cout << "Time: " << duration << " ms" << std::endl;

        closesocket(clientSocket);
        closesocket(serverSocket);

        WSACleanup();
        return result;
    }

    void Server::WSInit() {
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            std::cerr << "Failed to initialize winsock" << std::endl;
            exit(1);
        }
    }

    SOCKET Server::createSocket() {
        SOCKET s = socket(AF_INET, SOCK_STREAM, 0);
        if (s == INVALID_SOCKET) {
            std::cerr << "Failed to create socket" << std::endl;
            WSACleanup();
            exit(1);
        }
        return s;
    }

    sockaddr_in Server::bindSocket(SOCKET& socket) {
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

    void Server::listenOnSocket(SOCKET& socket) {
        if (listen(socket, SOMAXCONN) == SOCKET_ERROR) {
            std::cerr << "Failed to listen on socket" << std::endl;
            closesocket(socket);
            WSACleanup();
            exit(1);
        }
        std::cout << "Server is listening on port " << SERVER_PORT << std::endl;
    }

    void Server::acceptConnection(SOCKET serverSocket) {
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

    bool Server::askForConfirmation() {
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

    void Server::parseInput(char* inputPayload) {
        char digitalInputsStr[1024];
        char analogInputsStr[1024];
        std::memcpy(digitalInputsStr, inputPayload, inputBoolPayloadSize);
        std::memcpy(analogInputsStr, inputPayload + inputBoolPayloadSize, inputIntPayloadSize);

        int intArray[1024];
        for (int i = 0; i < analogInputs.size(); i++) {
            std::memcpy(&intArray[i], &analogInputsStr[i * sizeof(int)], sizeof(int));
            int value = intArray[i];
            analogInputs[i]->set(value);
        }

        for (int i = 0; i < inputBoolPayloadSize; i++) {
            unsigned char uc = static_cast<unsigned char>(digitalInputsStr[i]);
            for (int j = 7; j >= 0; j--) {
                bool value = (uc & (1 << j)) != 0;
                int valueIndex = i * 8 + (7 - j);
                if(valueIndex >= digitalInputs.size()) {
					return;
				}
                digitalInputs[valueIndex]->set(value);
            }
        }
    }

    void Server::serializeOutput(char* outputPayload) {
        unsigned char digitalOutputsStr[1024];
        unsigned char analogOutputsStr[1024];

        std::memset(digitalOutputsStr, 0, sizeof(digitalOutputsStr));
        std::memset(analogOutputsStr, 0, sizeof(analogOutputsStr));

        for (int i = 0; i < digitalOutputs.size(); i++) {
            int byteIndex = i / 8;
            int bitIndex = i % 8;

            bool value = digitalOutputs[i]->get();
            unsigned char uc = digitalOutputsStr[byteIndex];
            if (value) {
                uc |= (1 << 7 - i);
            }
            digitalOutputsStr[byteIndex] = uc;
        }

        for (int i = 0; i < analogOutputs.size(); i++) {
            int value = analogOutputs[i]->get();
            std::memcpy(&analogOutputsStr[i * sizeof(int)], &value, sizeof(int));
        }

        std::memcpy(outputPayload, digitalOutputsStr, outputBoolPayloadSize);
        std::memcpy(outputPayload + outputBoolPayloadSize, analogOutputsStr, outputIntPayloadSize);
    }

    bool Server::verifySequence(char* sequence) {
        return clientSequence == reinterpret_cast<unsigned int*>(sequence);
    }

    void Server::serializeSequence(char* sequence) {
	    std::memcpy(sequence, &serverSequence, SEQUENCE_SIZE);
    }

    void Server::sendData() {
	    char dataMessage[1024];
        int size = DATA_HEADER_LENGTH + outputPayloadSize;
        std::memset(dataMessage, 0, sizeof(dataMessage));
        std::memcpy(dataMessage, encryption::DATA_HEADER, std::strlen(encryption::DATA_HEADER));
		serializeSequence(dataMessage + std::strlen(encryption::DATA_HEADER));
        ++serverSequence;
		serializeOutput(dataMessage + DATA_HEADER_LENGTH);
        if (security >= ComSecurityLevel::ENCRYPTED) {
            crypto->encrypt(dataMessage, size);
            if (security == ComSecurityLevel::SECURE) {
			    std::memcpy(dataMessage + size, (crypto->sign(dataMessage)).c_str(), encryption::SIGNATURE_LENGTH);
                size += encryption::SIGNATURE_LENGTH;
            }
        }
		if (!sendToSocket(clientSocket, dataMessage, size)) {
			std::cerr << "Failed to send data" << std::endl;
		}
    }

    bool Server::readFromSocket(SOCKET socket, char* buffer) {
        int bytesRead;
        unsigned long availableBytes = 0;
        while (availableBytes < requiredBytes) {
            ioctlsocket(socket, FIONREAD, &availableBytes);
        }

        std::memset(buffer, 0, sizeof(buffer));
        bytesRead = recv(socket, buffer, requiredBytes, 0);
        if (bytesRead <= 0) {
            return false;
        }
        return true;
    }

    bool Server::authenticate(std::string authenticationMsg) {
        bool knownClient = crypto->checkForKnownKey(authenticationMsg.substr(std::strlen(encryption::AUTHENTICATION_HEADER), encryption::B64_KEY_LENGTH));
        
        if (!crypto->verifySignature(authenticationMsg.substr(0, 2 * (std::strlen(encryption::AUTHENTICATION_HEADER) + encryption::B64_KEY_LENGTH)), authenticationMsg.substr(authenticationMsg.size() - encryption::SIGNATURE_LENGTH, encryption::SIGNATURE_LENGTH))) {
            return false;
        }
        else if (!knownClient) {    
            if (askForConfirmation())
				crypto->saveClientPublicKey();
            else
				return false;
        }
        return true;
    }

    bool Server::sendToSocket(SOCKET socket, const char* message, int size)
    {
        int bytesSent = send(socket, message, size, 0);
        if (bytesSent == SOCKET_ERROR) {
            std::cerr << "Failed to send message: " << WSAGetLastError() << std::endl;
            return false;
        }
        std::cout << "Sent: " << message << std::endl;
        return true;
    }
    
    Server::~Server() {
        for (auto& dI : digitalInputs) {
            delete dI;
            dI = nullptr;
        }
        for (auto& dO : digitalOutputs) {
            delete dO;
            dO = nullptr;
        }
        for (auto& aI : analogInputs) {
            delete aI;
            aI = nullptr;
        }
        for (auto& aO : analogOutputs) {
            delete aO;
            aO = nullptr;
        }
    }
}