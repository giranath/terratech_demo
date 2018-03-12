#ifndef MMAP_DEMO_RSA_HPP
#define MMAP_DEMO_RSA_HPP

#include <cstdint>
#include <random>
#include <tuple>

#ifdef WIN32
#include <numeric>
#else
#include <experimental/numeric>
#endif

#include <algorithm>

namespace crypto {

inline bool is_prime(uint64_t number) {
    if(number == 2) return true;
    if(number < 2 || number % 2 == 0) return false;

    for(uint64_t i = 3; i < number; i += 2) {
        if(number % i == 0) return false;
    }

    return true;
}

template<typename Random>
uint64_t generate_prime(Random& rand, uint64_t min, uint64_t max) {
    std::uniform_int_distribution<uint64_t> dist(min, max);

    uint64_t prime;
    do {
        prime = dist(rand);
    } while(!is_prime(prime));

    return prime;
}

inline uint64_t mod_inverse(uint64_t a, uint64_t m)
{
    uint64_t m0 = m;
    int64_t y = 0, x = 1;

    if (m == 1)
        return 0;

    while (a > 1)
    {
        // q is quotient
        uint64_t q = a / m;
        int64_t t = m;

        // m is remainder now, process same as
        // Euclid's algo
        m = a % m, a = t;
        t = y;

        // Update y and x
        y = x - q * y;
        x = t;
    }

    // Make x positive
    if (x < 0)
        x += m0;

    return static_cast<uint64_t>(x);
}

template<typename Random>
class prime_generator {
    Random& rand;
public:
    explicit prime_generator(Random& rand)
    : rand(rand) {

    }

    uint64_t operator()(uint64_t min, uint64_t max) {
        return generate_prime<Random>(rand, min, max);
    }
};

template<typename Random>
class cached_prime {
    Random& rand;
    std::vector<uint64_t> primes;
public:
    template<typename It>
    cached_prime(Random& rand, It begin, It end)
    : rand{rand}
    , primes(begin, end) {
    }

    uint64_t operator()(uint64_t min, uint64_t max) {
        auto start_it = std::lower_bound(primes.begin(), primes.end(), min);
        auto end_it = std::upper_bound(primes.begin(), primes.end(), max);

        std::size_t start_index = static_cast<std::size_t>(std::distance(primes.begin(), start_it));
        std::size_t end_index = static_cast<std::size_t>(std::distance(primes.begin(), end_it));

        std::uniform_int_distribution<std::size_t> dist(start_index, end_index);

        uint64_t prime = primes[dist(rand)];
        return prime;
    }
};

namespace rsa {
using key = std::tuple<uint64_t, uint64_t>;

struct key_pair {
    key private_key;
    key public_key;

    key_pair(key priv, key pub)
    : private_key{priv}
    , public_key{pub} {

    }
};

template<uint64_t MIN, uint64_t MAX, typename Generator>
key_pair generate_keys(Generator prime_gen) {
    auto p = prime_gen(MIN, MAX);
    auto q = prime_gen(MIN, MAX);

    auto n = p * q;

    uint64_t lambda = std::experimental::lcm(p - 1, q - 1);
    uint64_t e;
    do {
        e = prime_gen(1, lambda - 1);
    } while (lambda % e == 0);

    uint64_t d = mod_inverse(e, lambda);

    return key_pair(std::make_tuple(n, d), std::make_tuple(n, e));
}

}
}

#endif //MMAP_DEMO_RSA_HPP
