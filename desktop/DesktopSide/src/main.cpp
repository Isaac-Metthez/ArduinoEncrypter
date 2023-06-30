#include <iostream>
#include <string>

#include "../include/tcpServer.h"

int main() {

    try {
        Com::TCPServer::start();
    }
    catch (const std::exception& e) {
	    std::cout << e.what();
    }
    return 0;
}