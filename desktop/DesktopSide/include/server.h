#pragma once

#include <iostream>
#include <vector>
#include <algorithm>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "crypto.h"
#include "utils.h"
#include "IO.h"

namespace com {
	constexpr auto SERVER_PORT = 20582;
	constexpr auto DATA_HEADER_LENGTH = 16;
	constexpr auto AUTH_MESSAGE_LENGTH = 160;
	constexpr auto BIT_PER_BLOCK = 128;
	constexpr auto INT_PER_BLOCK = 4;
	constexpr auto SEQUENCE_SIZE = 12;

	enum class ComSecurityLevel { SIMPLE , ENCRYPTED, SECURE };

	class Server
	{
		private:
			SOCKET serverSocket;
			SOCKET clientSocket;
			sockaddr_in serverAddress;
			sockaddr_in clientAddress;
			unsigned int requiredBytes;
			int inputBoolPayloadSize;
			int inputIntPayloadSize;
			int inputPayloadSize;
			int outputBoolPayloadSize;
			int outputIntPayloadSize;
			int outputPayloadSize;
			bool sendOutputs;
			ComSecurityLevel security;
			encryption::Crypto* crypto;
			std::vector<digitalInput*> digitalInputs;
			std::vector<analogInput*> analogInputs;
			std::vector<digitalOutput*> digitalOutputs;
			std::vector<analogOutput*> analogOutputs;
			utils::Huge3Uint serverSequence;
			utils::Huge3Uint clientSequence;

			void WSInit();
			SOCKET createSocket();
			sockaddr_in bindSocket(SOCKET& socket);
			void listenOnSocket(SOCKET& socket);
			void acceptConnection(SOCKET serverSocket);
			bool askForConfirmation();
			bool readFromSocket(SOCKET socket, char* buffer);
			bool authenticate(std::string authenticationStr);
			bool sendToSocket(SOCKET socket, const char* message, int size);
			void parseInput(char* inputPayload);
			void serializeOutput(char* outputPayload);
			bool verifySequence(char* sequence);
			void serializeSequence(char* sequence);
			void sendData();
		public:
			~Server();
			bool start();
			void AddInput(digitalInput* Input) { digitalInputs.push_back(Input); }
			void AddInput(analogInput* Input) { analogInputs.push_back(Input); }
			void AddOutput(digitalOutput* Output) { digitalOutputs.push_back(Output); }
			void AddOutput(analogOutput* Output) { analogOutputs.push_back(Output); }
	};
}