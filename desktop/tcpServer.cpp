#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include <cryptopp/osrng.h>
#include <cryptopp/xed25519.h>
#include <cryptopp/files.h>
#include <cryptopp/filters.h>
#include <cryptopp/base64.h>
#include <cryptopp/hex.h>

#define SERVER_PORT 20582


void createSocket(int& s){
    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) {
        std::cerr << "Failed to create socket" << std::endl;
        exit(1);
    }
}

sockaddr_in bindSocket(int socket){
    sockaddr_in address;
    std::memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(SERVER_PORT);
    if (bind(socket, (sockaddr*)&address, sizeof(address)) < 0) {
        std::cerr << "Failed to bind socket" << std::endl;
        exit(1);
    }
    std::cout << "Socket bound on port " << SERVER_PORT << std::endl << "Listening..." << std::endl;
    return address;
}

void listenOnSocket(int socket){
    if (listen(socket, 10) < 0) {
        std::cerr << "Failed to listen for connections" << std::endl;
        exit(1);
    }
}

int readSocket(char[]& buffer){
    
}

std::string base64Decode(std::string str){
    std::string decodedStr;
    StringSource(str, true,
        new Base64Decoder(new StringSink(decodedStr)));
    return decodedStr;  
}

std::string savePublicKey(std::string keyData){

    
}

bool checkForKnownPublicKey(std::string hexPublicKey){
    return true;
}


int main() {
    int serverSocket;
    createSocket(serverSocket);
    sockaddr_in serverAddress = bindSocket(serverSocket);
    listenOnSocket(serverSocket);   

    // Accept incoming connections and handle them
    while (true) {
        sockaddr_in clientAddress;
        socklen_t clientAddressSize = sizeof(clientAddress);
        int clientSocket = accept(serverSocket, (sockaddr*)&clientAddress, &clientAddressSize);
        if (clientSocket < 0) {
            std::cerr << "Failed to accept connection" << std::endl;
            break;
        }
	
        std::cout << "Client connected" << std::endl;

            // Read and write data on the client socket
        while(true) {
            char buffer[2048];
            std::memset(buffer, 0, sizeof(buffer));
            int bytesRead = read(clientSocket, buffer, sizeof(buffer));
            if (bytesRead < 0) {
                std::cerr << "Failed to read from socket" << std::endl;
                break;
            }

            if(bytesRead == 0){
                std::cout << "Client closed connection" << std::endl;
                break;
            }
            std::string rawData(buffer);
             
            std::string newKey = saveKey(buffer);
            if(checkForKnownKey()){
                    std::cout << "An unknown client has sent a public key. Accept [y/n] " << std::endl;
                    char c;
                    std::cin >> c;
                    if(c == 'y')
                        std::cout << "OK";
                    else
                       break;
            }

            std::cout << "Received message: " << buffer << std::endl;

            const char* response = "Hello, client!\n";
            int bytesWritten = write(clientSocket, response, std::strlen(response));
            if (bytesWritten < 0) {
                std::cerr << "Failed to write to socket" << std::endl;
                break;
            }
        }

        close(clientSocket);

        std::cout << "Client disconnected" << std::endl;
    }

    return 0;
}
