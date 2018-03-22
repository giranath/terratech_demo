#include "aes.hpp"

namespace crypto { namespace aes {



bytes encrypt(bytes plaintext, key key) {
    bytes ciphertext;
    ciphertext.reserve(plaintext.size() + 16);

    bytes iv(16, 0);

    CryptoPP::AES::Encryption aes_encryption(&key.front(), key.size());
    CryptoPP::CBC_Mode_ExternalCipher::Encryption cbc_encryption(aes_encryption, &iv.front());

    CryptoPP::StreamTransformationFilter filter(cbc_encryption, new iterator_sink(std::back_inserter(ciphertext)));
    filter.Put(&plaintext.front(), plaintext.size());
    filter.MessageEnd();

    return ciphertext;
}

bytes decrypt(bytes ciphertex, key key) {
    bytes plaintext;
    plaintext.reserve(ciphertex.size());

    bytes iv(16, 0);

    CryptoPP::AES::Decryption aes_decryption(&key.front(), key.size());
    CryptoPP::CBC_Mode_ExternalCipher::Decryption cbc_decryption(aes_decryption, &iv.front());

    CryptoPP::StreamTransformationFilter stfDecryptor(cbc_decryption, new iterator_sink(std::back_inserter(plaintext)));
    stfDecryptor.Put(&ciphertex.front(), ciphertex.size());
    stfDecryptor.MessageEnd();

    return plaintext;
}

}}