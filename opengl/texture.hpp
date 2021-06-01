#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <glad/glad.h>

#include <vector>

namespace rjs {
    class texture {
    public:
        static std::vector<unsigned> generate(std::size_t n){
            std::vector<unsigned> to_return;
            to_return.resize(n);
            glGenTextures(n, to_return.data());
            return to_return;
        }


    };
}
#endif