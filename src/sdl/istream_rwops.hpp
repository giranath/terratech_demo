#ifndef MMAP_DEMO_ISTREAM_RWOPS_HPP
#define MMAP_DEMO_ISTREAM_RWOPS_HPP

#include <SDL.h>
#include <iostream>
#include <fstream>

namespace sdl {

SDL_RWops* make_from_fstream(std::fstream&& stream);

}

#endif //MMAP_DEMO_ISTREAM_RWOPS_HPP
