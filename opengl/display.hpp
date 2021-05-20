#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <thread>
#include <chrono>
#include <iostream>
#include <fstream>
#include <cstring>
#include "shader.hpp"
#include "window.hpp"

using namespace std::chrono_literals;

float vertices[] = {
    -0.5f, -0.5f, 0.0f,
     0.5f, -0.5f, 0.0f,
     0.0f,  0.5f, 0.0f
}; 

int run(int argc, char** argv){
    rjs::window window("Conway Visualization", 300, 100, 400, 400);

    window.on_draw([]{
        glClear(GL_COLOR_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLES, 0, 3);
    });
    
    uint VAO;
    glGenVertexArrays(1, &VAO); 
    glBindVertexArray(VAO);

    uint VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    rjs::shader program("shaders/vertex.vs.glsl", "shaders/fragment.fs.glsl");
    if(!program.good()){
        glfwTerminate();
        return -1;
    }
    program.use();

    window.run();

    glfwTerminate();
    return 0;
}