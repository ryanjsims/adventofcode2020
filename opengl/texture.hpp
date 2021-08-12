#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <glad/glad.h>
#include <stb_image.h>

#include <vector>

namespace rjs {
    class texture {
    private:
        unsigned id;
    public:
        static std::vector<unsigned> generate(std::size_t n){
            std::vector<unsigned> to_return;
            to_return.resize(n);
            glGenTextures(n, to_return.data());
            return to_return;
        }

        texture(unsigned id = 0){
            if(id == 0){
                id = generate(1)[0];
            }
            this->id = id;
        }

        bool load_image(std::string filename){
            int w, h, ch;
            unsigned char* data = stbi_load(filename.c_str(), &w, &h, &ch, 4);
            if(data == nullptr){
                std::cerr << "Failed to load " << filename << ": " << stbi_failure_reason() << std::endl;
                return false;
            }
            glBindTexture(GL_TEXTURE_2D, id);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            glBindTexture(GL_TEXTURE_2D, 0);
            stbi_image_free(data);
            return true;
        }

        void bind(){
            glBindTexture(GL_TEXTURE_2D, id);
        }
    };
}
#endif