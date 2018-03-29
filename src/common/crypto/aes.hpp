#ifndef MMAP_DEMO_AES_HPP
#define MMAP_DEMO_AES_HPP

#ifndef NCRYPTO

#include <cstdint>
#include <vector>

#include <random>

#if defined(__APPLE__)
#include <cryptopp/aes.h>
#include <cryptopp/modes.h>
#include <cryptopp/filters.h>
#elif defined(WIN32)
#else
#include <crypto++/aes.h>
#include <crypto++/modes.h>
#include <crypto++/filters.h>
#endif

#include "iterator_sink.hpp"

namespace crypto { namespace aes {

using key = std::vector<uint8_t>;
using bytes = std::vector<uint8_t>;

template<typename InputIt, typename OutputIt>
OutputIt encrypt(key k, InputIt begin, InputIt end, OutputIt out) {
    bytes plaintext(begin, end);

    bytes iv(16, 0);

    CryptoPP::AES::Encryption aes_encryption(&k.front(), k.size());
    CryptoPP::CBC_Mode_ExternalCipher::Encryption cbc_encryption(aes_encryption, &iv.front());

    CryptoPP::StreamTransformationFilter filter(cbc_encryption, new crypto::iterator_sink<OutputIt>(out));
    filter.Put(&plaintext.front(), plaintext.size());
    filter.MessageEnd();

    return out;
}

template<typename InputIt, typename OutputIt>
OutputIt decrypt(key k, InputIt begin, InputIt end, OutputIt out) {
    bytes ciphertext(begin, end);
    bytes iv(16, 0);

    CryptoPP::AES::Decryption aes_decryption(&k.front(), k.size());
    CryptoPP::CBC_Mode_ExternalCipher::Decryption cbc_decryption(aes_decryption, &iv.front());

    CryptoPP::StreamTransformationFilter stfDecryptor(cbc_decryption, new iterator_sink<OutputIt>(out));
    stfDecryptor.Put(&ciphertext.front(), ciphertext.size());
    stfDecryptor.MessageEnd();

    return out;
}

template<typename random, std::size_t bit_len = 256>
key make_key(random& rd) {
    const std::size_t block_count = bit_len / 8;

    key k(block_count, 0);

    std::uniform_int_distribution<uint8_t> block_distrib(std::numeric_limits<uint8_t>::min(), std::numeric_limits<uint8_t>::max());
    for(std::size_t i = 0; i < block_count; ++i) {
        k[i] = block_distrib(rd);
    }

    return k;
}

}}
#endif
#endif //MMAP_DEMO_AES_HPP
