// Copyright 2022 Ruslan Kulagin

#ifndef INCLUDE_HASH_HPP_
#define INCLUDE_HASH_HPP_

#include <digest.hpp>
#include <openssl/md5.h>
#include <vector>

namespace NHash {
    using byte_type = unsigned char;
    constexpr const unsigned DIGEST_SIZE = MD5_DIGEST_LENGTH;

    inline std::vector<byte_type> Hash(const byte_type *value, size_t messageLength) {
        std::vector<byte_type> digest(DIGEST_SIZE);
        MD5(value, messageLength, digest.data());
        return digest;
    }


} // namespace NHash

#endif //INCLUDE_HASH_HPP_
