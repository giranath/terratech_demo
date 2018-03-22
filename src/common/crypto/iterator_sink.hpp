#ifndef MMAP_DEMO_ITERATOR_SINK_HPP
#define MMAP_DEMO_ITERATOR_SINK_HPP

#if defined(__APPLE__)
#include <cryptopp/filters.h>
#elif defined(WIN32)
#else
#include <crypto++/filters.h>
#endif

namespace crypto {

template<typename It>
class iterator_sink : public CryptoPP::Bufferless<CryptoPP::Sink> {
    It iterator;
public:
    explicit iterator_sink(It out) : iterator(out) {

    }

    std::size_t Put2(const CryptoPP::byte* byte, std::size_t length, int /*messageEnd*/, bool /*blocking*/) {
        iterator = std::copy(byte, byte + length, iterator);

        return 0;
    }
};

}

#endif //MMAP_DEMO_HEADER_HPP
