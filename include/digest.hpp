// Copyright 2022 Ruslan Kulagin

#ifndef INCLUDE_DIGEST_HPP_
#define INCLUDE_DIGEST_HPP_

#include <cstdint>
#include <array>

template<uint8_t DIGEST_SIZE>
class TDigest {
public:
    using byte_type = unsigned char;

    [[nodiscard]] const byte_type *data() const {
        return digest.data();
    }
        
    [[nodiscard]] byte_type *data() {
        return digest.data();
    }

    typename std::array<byte_type, DIGEST_SIZE>::iterator begin(){
        return digest.begin();
    }


private:

    std::array<byte_type, DIGEST_SIZE> digest;

};

#endif //INCLUDE_DIGEST_HPP_
