#include <blockchain.hpp>
#include <iostream>

int main() {
    TBlockchain blockchain;
    while (true) {
        std::string message;
        std::getline(std::cin, message);
        blockchain.AddMessage(message);
        std::cout << blockchain << std::endl;
    }
    return 0;
}
