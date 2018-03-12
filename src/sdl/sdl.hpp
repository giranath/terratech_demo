#ifndef MMAP_DEMO_SDL_HPP
#define MMAP_DEMO_SDL_HPP

#include <SDL.h>
#ifdef WIN32
#include <SDL_image.h>
#else
#include <SDL2/SDL_image.h>
#endif

#include "context.hpp"
#include "poll_event_iterator.hpp"
#include "window.hpp"

#endif
