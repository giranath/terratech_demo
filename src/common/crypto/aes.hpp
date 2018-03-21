#ifndef MMAP_DEMO_AES_HPP
#define MMAP_DEMO_AES_HPP

#include <cstdint>
#include <vector>

namespace crypto { namespace aes {

using key = std::vector<uint8_t>;
using bytes = std::vector<uint8_t>;

bytes encrypt(bytes plaintext, key key);
bytes decrypt(bytes ciphertex, key key);

}}

#endif //MMAP_DEMO_AES_HPP
