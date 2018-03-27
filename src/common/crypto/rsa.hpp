#ifndef MMAP_DEMO_RSA_HPP
#define MMAP_DEMO_RSA_HPP

#ifndef NCRYPTO

#if defined(__APPLE__)
#include <cryptopp/rsa.h>
#include <cryptopp/osrng.h>
#include <cryptopp/files.h>
#elif defined(WIN32)
#else
#include <crypto++/rsa.h>
#include <crypto++/osrng.h>
#include <crypto++/files.h>
#endif

#include "iterator_sink.hpp"

namespace crypto { namespace rsa {

using bytes = std::vector<uint8_t>;

using private_key = CryptoPP::RSA::PrivateKey;
using public_key = CryptoPP::RSA::PublicKey;

template<typename key>
const key load_key(const char* path) {
    key k;
    CryptoPP::ByteQueue queue;
    CryptoPP::FileSource file(path, true);
    file.TransferTo(queue);
    queue.MessageEnd();

    k.Load(queue);

    return k;
}

template<typename InputIt, typename OutputIt>
OutputIt encrypt(public_key key, InputIt begin, InputIt end, OutputIt out) {
    bytes plaintext(begin, end);
    CryptoPP::AutoSeededRandomPool prng;

    CryptoPP::RSAES_OAEP_SHA_Encryptor e(key);
    CryptoPP::ArraySource(&plaintext.front(), plaintext.size(), true,
                          new CryptoPP::PK_EncryptorFilter(prng, e, new iterator_sink<OutputIt>(out)));

    return out;
}

template<typename InputIt, typename OutputIt>
OutputIt decrypt(private_key key, InputIt begin, InputIt end, OutputIt out) {
    bytes ciphertext(begin, end);
    CryptoPP::AutoSeededRandomPool prng;

    CryptoPP::RSAES_OAEP_SHA_Decryptor d(key);
    CryptoPP::ArraySource(&ciphertext.front(), ciphertext.size(), true,
                          new CryptoPP::PK_DecryptorFilter(prng, d, new iterator_sink<OutputIt>(out)));

    return out;
}

}}

#endif

#endif //MMAP_DEMO_RSA_HPP
