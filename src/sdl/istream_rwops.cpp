#include "istream_rwops.hpp"

#include <new>
#include <SDL_rwops.h>

namespace sdl {

namespace internal {
    struct fstream_context {
        std::fstream stream;

        fstream_context(std::fstream&& stream)
        : stream{std::move(stream)} {

        }

        static Sint64 size(SDL_RWops* ops) {
            fstream_context* ctx = static_cast<fstream_context*>(ops->hidden.unknown.data1);

            auto pos = ctx->stream.tellg();
            ctx->stream.seekg(0, std::ios_base::end);
            auto last = ctx->stream.tellg();
            ctx->stream.seekg(pos, std::ios_base::beg);

            return last - pos;
        }

        static Sint64 seek(SDL_RWops* ops, Sint64 offset, int pos) {
            fstream_context* ctx = static_cast<fstream_context*>(ops->hidden.unknown.data1);

            switch(pos) {
                case RW_SEEK_SET:
                    ctx->stream.seekg(offset, std::ios_base::beg);
                    break;
                case RW_SEEK_END:
                    ctx->stream.seekg(offset, std::ios_base::end);
                    break;
                case RW_SEEK_CUR:
                    ctx->stream.seekg(offset, std::ios_base::cur);
                    break;
                default:
                    break;
            }

            return ctx->stream.tellg();
        }

        static size_t read(SDL_RWops* ops, void* buffer, size_t size, size_t maxnum) {
            fstream_context* ctx = static_cast<fstream_context*>(ops->hidden.unknown.data1);

            char* bytes = static_cast<char*>(buffer);
            std::size_t i = 0;
            for(; i < maxnum && ctx->stream.good(); ++i) {
                ctx->stream.read(&bytes[i * size], size);
            }

            if(ctx->stream.eof()) {
                return EOF;
            }

            return i;
        }

        static size_t write(SDL_RWops* ops, const void* data, size_t size, size_t num) {
            fstream_context* ctx = static_cast<fstream_context*>(ops->hidden.unknown.data1);

            const char* bytes = static_cast<const char*>(data);
            std::size_t i = 0;
            for(; i < num && ctx->stream.good(); ++i) {
                ctx->stream.write(&bytes[i * size], size);
            }

            return i;
        }

        static int close(SDL_RWops* ops) {
            fstream_context* ctx = static_cast<fstream_context*>(ops->hidden.unknown.data1);

            ctx->stream.close();

            delete ctx;
            SDL_FreeRW(ops);

            return 0;
        }
    };

}

SDL_RWops* make_from_ifstream(std::fstream&& stream) {
    internal::fstream_context* ctx = new(std::nothrow) internal::fstream_context{std::move(stream)};

    if(!ctx) {
        return nullptr;
    }

    SDL_RWops* ops = SDL_AllocRW();
    if(ops) {
        ops->type = SDL_RWOPS_UNKNOWN;
        ops->hidden.unknown.data1 = ctx;
        ops->size = internal::fstream_context::size;
        ops->close = internal::fstream_context::close;
        ops->read = internal::fstream_context::read;
        ops->write = internal::fstream_context::write;
        ops->seek = internal::fstream_context::seek;
    }

    return ops;
}

}