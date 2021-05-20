#ifndef VBUFFER_HPP
#define VBUFFER_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

namespace rjs {
    class vbuffer{
    public:
    //TODO: implement this when
        vbuffer(){
            std::cerr << "Warning: vbuffer is unimplemented!" << std::endl;
        }
    private:
        unsigned id;
    };
}
#endif