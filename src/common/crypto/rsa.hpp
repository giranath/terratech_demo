#ifndef MMAP_DEMO_RSA_HPP
#define MMAP_DEMO_RSA_HPP

#ifndef NCRYPTO

#if defined(__APPLE__)
#include <cryptopp/rsa.h>
#include <cryptopp/osrng.h>
#include <cryptopp/files.h>
#include <cryptopp/filters.h>
#else
#include <crypto++/rsa.h>
#include <crypto++/osrng.h>
#include <crypto++/files.h>
#include <crypto++/filters.h>
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

template<typename InputIt, typename OutputIt>
OutputIt sign(private_key key, InputIt begin, InputIt end, OutputIt out) {
    bytes plaintext(begin, end);
    CryptoPP::AutoSeededRandomPool prng;

    CryptoPP::RSASSA_PKCS1v15_SHA_Signer s(key);
    CryptoPP::ArraySource(&plaintext.front(), plaintext.size(), true,
                          new CryptoPP::SignerFilter(prng, s,
                                                     new iterator_sink<OutputIt>(out)));

    return out;
}

template<typename InputIt, typename SignatureIt>
bool verify(public_key key, InputIt data_begin, InputIt data_end, SignatureIt signature_begin, SignatureIt signature_end) {
    bytes ciphertext(data_begin, data_end);
    std::copy(signature_begin, signature_end, std::back_inserter(ciphertext));

    CryptoPP::AutoSeededRandomPool prng;

    CryptoPP::RSASSA_PKCS1v15_SHA_Verifier v(key);
    bool result = false;
    CryptoPP::ArraySource(&ciphertext.front(), ciphertext.size(), true,
                          new CryptoPP::SignatureVerificationFilter(v,
                                                                    new CryptoPP::ArraySink(reinterpret_cast<uint8_t*>(&result), sizeof(result)),
                                                                    CryptoPP::SignatureVerificationFilter::PUT_RESULT | CryptoPP::SignatureVerificationFilter::SIGNATURE_AT_END
                                                                    ));

    return result;
}

}}

#endif

#endif //MMAP_DEMO_RSA_HPP
