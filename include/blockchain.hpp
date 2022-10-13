// Copyright 2022 Ruslan Kulagin

#ifndef INCLUDE_BLOCKCHAIN_HPP_
#define INCLUDE_BLOCKCHAIN_HPP_

#include <hash.hpp>
#include <digest.hpp>

#include <boost/fiber/all.hpp>
#include <boost/fiber/fiber.hpp>
#include <boost/fiber/future.hpp>

#include <cstdint>
#include <string>
#include <list>
#include <memory>
#include <utility>
#include <vector>
#include <iomanip>
#include <future>
#include <iostream>

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

    static bool IsValidHash(const digest_type &hash) {
//        return hash[hash.size() - 1] == 0 && hash[hash.size() - 2] == 0 && hash[hash.size() - 3] == 0;
        return hash[hash.size() - 1] == 0 && hash[hash.size() - 3] == 0;
    }

    static std::unique_ptr<TNode> GenerateBlock(const std::string &message, const digest_type &digest) {
        constexpr byte_type THREADS_NUMBER = 16;
        std::vector<boost::fibers::future<std::unique_ptr<TNode>>> futures;
        futures.reserve(THREADS_NUMBER);
        for (byte_type i = 0; i < THREADS_NUMBER; ++i) {
            futures.push_back(boost::fibers::async(boost::fibers::launch::post, BruteNonce, message, digest, THREADS_NUMBER, i));
        }
        size_t i = 0;
        while (futures[i].wait_until(std::chrono::system_clock::time_point::min()) != boost::fibers::future_status::ready) {
            i = (i + 1) % THREADS_NUMBER;
        }
        std::cout << i << std::endl;
        auto result = futures[i].get();
        futures.clear();
        return result;
    }

    static std::unique_ptr<TNode>
    BruteNonce(const std::string &message, const digest_type &digest, byte_type delta, byte_type init) {
        nonce_type nonce = {init};
        std::unique_ptr<data_type> msg;
        std::vector<byte_type> hash;
        while (true) {
            if (CheckBlock(message, digest, nonce, hash)) {
                break;
            } else {
                IncreaseNonce(nonce, delta);
            }
        }
        return std::make_unique<TNode>(message, hash, nonce);
    }

    static bool
    CheckBlock(const std::string &message, const digest_type &digest, const nonce_type &nonce, digest_type &hash) {
        const auto msg = GenerateMessageBytes(digest, message, nonce);
        hash = NHash::Hash(msg->data(), msg->size());
        return IsValidHash(hash);
    }

    static void IncreaseNonce(nonce_type &nonce, uint64_t delta) {
        nonce.front() += delta;
        if (nonce.front() < delta) {
            if (nonce.size() == 1){
                nonce.push_back(0);
            }
            for (auto it = nonce.begin() + 1; it != nonce.end() && *it == 0;) {
                ++it;
                if (it == nonce.end()) {
                    nonce.push_back(0);
                    break;
                } else {
                    ++(*it);
                }
            }
        }
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