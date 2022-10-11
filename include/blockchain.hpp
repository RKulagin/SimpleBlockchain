// Copyright 2022 Ruslan Kulagin

#ifndef INCLUDE_BLOCKCHAIN_HPP_
#define INCLUDE_BLOCKCHAIN_HPP_

#include <hash.hpp>
#include <digest.hpp>

#include <cstdint>
#include <string>
#include <list>
#include <memory>
#include <utility>
#include <vector>
#include <iomanip>

class TBlockchain {
public:

    TBlockchain() : TBlockchain(42) {}

    explicit TBlockchain(uint64_t init) : initHash(NHash::Hash(reinterpret_cast<const byte_type *>(&init), 1)) {
    }

    TBlockchain(TBlockchain &) = delete;

    TBlockchain(TBlockchain &&) = default;

    TBlockchain &operator=(TBlockchain &) = delete;

    TBlockchain &operator=(TBlockchain &&) = default;

    void AddMessage(const std::string &message) {
        if (chain.empty()) {
            chain.push_back(GenerateBlock(message, initHash));
        } else {
            chain.push_back(GenerateBlock(message, chain.back()->digest_));
        }
    }

private:
    using byte_type = unsigned char;
    using data_type = std::vector<byte_type>;
    using nonce_type = data_type;
    using digest_type = data_type;

    struct TNode {
        std::string message_;
        digest_type digest_;
        nonce_type nonce_;

        TNode(std::string message, digest_type digest, nonce_type nonce) : message_(std::move(message)),
                                                                           digest_(std::move(digest)),
                                                                           nonce_(std::move(nonce)) {}
    };

    static bool IsValidHash(const digest_type & hash){
        return hash[hash.size() - 1] == 0 && hash[hash.size() - 2] == 0 && hash[hash.size() - 3] == 0;
    }

    std::unique_ptr<TNode> GenerateBlock(const std::string &message, const digest_type &digest) {
        nonce_type nonce = {0};
        std::unique_ptr<data_type> msg;
        std::vector<byte_type> hash;
        while (true) {
            msg = GenerateMessageBytes(digest, message, nonce);
            hash = NHash::Hash(msg->data(), msg->size());
            if (IsValidHash(hash)) {
                break;
            } else {
                ++nonce.front();
                for (auto it = nonce.begin(); it!= nonce.end() && *it == 0; ) {
                    ++it;
                    if (it == nonce.end()){
                        nonce.push_back(0);
                        break;
                    }
                    else{
                        ++(*it);
                    }
                }
            }
        }
        return std::make_unique<TNode>(message, hash, nonce);
    }

    static inline std::unique_ptr<data_type>
    GenerateMessageBytes(const digest_type &digest, const std::string &message, nonce_type nonce) {
        auto bytes = std::make_unique<data_type>(digest.size() + message.size() + nonce.size() + 1);
//        auto bytes = std::make_unique<data_type>(message.size());
        std::copy(digest.begin(), digest.end(), bytes->begin());
//        std::copy(message.begin(), message.end()-1, bytes->begin() + digest.size());
        std::copy(nonce.begin(), nonce.end(), bytes->begin() + digest.size());
        std::copy(message.begin(), message.end(), bytes->begin() + digest.size() + nonce.size());
        return bytes;
    }


    digest_type initHash;
    std::list<std::unique_ptr<TNode>> chain;

    friend std::ostream &operator<<(std::ostream &out, const TBlockchain &chain);
};

std::ostream &operator<<(std::ostream &out, const TBlockchain &blockchain) {
    for (const auto &element: blockchain.chain) {
        out << "{msg: \"" << element->message_ << "\", digest: ";
        for (unsigned char i : element->digest_) {
            out << std::setw(2) << std::setfill('0') << std::hex << (int) i;
        }
        out << ", nonce: ";
        for (auto i : element->nonce_) {
            out << std::setw(2) << std::setfill('0') << std::hex << (int) i;
        }
        out << std::endl;

    }
    return out;
}


#endif // INCLUDE_BLOCKCHAIN_HPP_