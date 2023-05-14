#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

int main() {
    // Create a TCP socket
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        std::cerr << "Failed to create socket" << std::endl;
        return 1;
    }

    // Bind the socket to a port
    struct sockaddr_in serverAddress;
    std::memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(12345);
    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        std::cerr << "Failed to bind socket" << std::endl;
        return 1;
    }
    std::cout << "Socket bound on port 12345" << std::endl << "Listening..." << std::endl;

    // Listen for incoming connections
    if (listen(serverSocket, 10) < 0) {
        std::cerr << "Failed to listen for connections" << std::endl;
        return 1;
    }

    // Accept incoming connections and handle them
    while (true) {
        struct sockaddr_in clientAddress;
        socklen_t clientAddressSize = sizeof(clientAddress);
        int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientAddressSize);
        if (clientSocket < 0) {
            std::cerr << "Failed to accept connection" << std::endl;
            break;
        }
	
	std::cout << "Client connected" << std::endl;

        // Read and write data on the client socket
	while(true) {
	    char buffer[256];
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
