#include "aes.hpp"
#include <crypto++/aes.h>
#include <crypto++/modes.h>
#include <crypto++/filters.h>

namespace crypto { namespace aes {

bytes encrypt(bytes plaintext, key key) {
    bytes ciphertext(plaintext.size() + 16, 0);
    bytes iv(16, 0);

    CryptoPP::AES::Encryption aes_encryption(&key.front(), key.size());
    CryptoPP::CBC_Mode_ExternalCipher::Encryption cbc_encryption(aes_encryption, &iv.front());

    CryptoPP::StreamTransformationFilter filter(cbc_encryption, new CryptoPP::ArraySink(&ciphertext.front(), ciphertext.size()));
}

bytes decrypt(bytes ciphertex, key key) {

}

}}