#include "aes.hpp"
#include <crypto++/aes.h>
#include <crypto++/modes.h>

namespace crypto { namespace aes {

bytes encrypt(bytes plaintext, key key) {
    bytes iv(CryptoPP::AES::BLOCKSIZE, 0);

    CryptoPP::AES::Encryption aes_encryption(&key.front(), key.size());
    //CryptoPP::CBC_Mode_ExternalCipher::Encryption cbc_encryption(aes_encryption, &iv.front(), iv.size());
}

bytes decrypt(bytes ciphertex, key key) {

}

}}