#include <blockchain.hpp>
#include <iostream>

void BlockChainServer() {
    TBlockchain blockchain;
    while (true) {
        std::string message;
        std::getline(std::cin, message);
        auto t = std::thread([&blockchain, message]() {
            blockchain.AddMessage(message);
        });
        std::cout << "Hashing, please wait." << std::endl;
        t.join();
        std::cout << blockchain << std::endl;
    }
}

int main() {
    BlockChainServer();
    return 0;
}
