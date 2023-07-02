#include <iostream>
#include <string>

#include "../include/server.h"

int main() {
    com::Server server;
    bool send = true, led = true;
    int counter = 0;
    server.AddInput(new com::digitalInput(led));
    server.AddInput(new com::analogInput(counter));
    server.AddOutput(new com::digitalOutput(send));
    server.AddOutput(new com::digitalOutput(led));

    try {
        while (true) {
            if (!server.start())
                break;
        }
    }
    catch (const std::exception& e) {
	    std::cout << e.what();
    }
    system("pause");
    return 0;
}