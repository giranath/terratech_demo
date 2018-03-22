#ifndef MMAP_DEMO_ITERATOR_SINK_HPP
#define MMAP_DEMO_ITERATOR_SINK_HPP

#ifndef NCRYPTO

#if defined(__APPLE__)
#include <cryptopp/filters.h>
#elif defined(WIN32)
#else
#include <crypto++/filters.h>
#endif

namespace crypto {

#ifdef CRYPTOPP_NO_GLOBAL_BYTE
using crypto_byte = CryptoPP::byte;
#else
using crypto_byte = byte;
#endif

template<typename It>
class iterator_sink : public CryptoPP::Bufferless<CryptoPP::Sink> {
    It iterator;
public:
    explicit iterator_sink(It out) : iterator(out) {

    }

    std::size_t Put2(const crypto_byte* byte, std::size_t length, int /*messageEnd*/, bool /*blocking*/) {
        iterator = std::copy(byte, byte + length, iterator);

        return 0;
    }
};

}

#endif
#endif //MMAP_DEMO_HEADER_HPP
