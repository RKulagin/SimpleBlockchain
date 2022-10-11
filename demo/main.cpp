#include <blockchain.hpp>
#include <iostream>

using byte = unsigned char;

int main() {
    TBlockchain blockchain;
    while (true) {
        std::string message;
        std::getline(std::cin, message)sou  ;
        blockchain.AddMessage(message);
        std::cout << blockchain << std::endl;
    }
    return 0;
}
