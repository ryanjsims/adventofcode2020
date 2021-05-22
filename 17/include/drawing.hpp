#ifndef DRAWING_HPP
#define DRAWING_HPP

#include <iostream>
#include <unordered_map>
#include <vector>
#include <list>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "coordinate.hpp"
#include "vertex.hpp"
#include "opengl/window.hpp"
#include "opengl/shader.hpp"

namespace std {
    template <>
    struct hash<pair<int,int>> {
        size_t operator()(const pair<int,int>& k) const {
            return (size_t)k.first ^ (size_t)k.second << 16;
        }
    };
}

glm::mat4 model(1.0f), view(1.0f);
int currW = 0, step = 0;
double last_xpos, last_ypos, rotx, roty;
std::unordered_map<int, bool> keys_down;

std::pair<std::vector<int>, std::vector<float>> get_mesh(std::unordered_map<coordinate, bool> world, int w);

void load_vertices(std::unordered_map<coordinate, bool> world, std::pair<coordinate, coordinate> AABB, int iteration, 
                   std::unordered_map<std::pair<int, int>, std::pair<uint, int>>& VAOs, 
                   std::vector<uint>& VBOs, 
                   std::vector<uint>& EBOs){
    int wRange = AABB.second.w - AABB.first.w + 1;
    uint *cube_vaos = new uint[wRange];
    uint *cube_vbos = new uint[wRange];
    uint *cube_ebos = new uint[wRange];
    glGenVertexArrays(wRange, cube_vaos);
    glGenBuffers(wRange, cube_vbos);
    glGenBuffers(wRange, cube_ebos);
    for(int j = 0; j < wRange; j++){
        auto mesh_vectors = get_mesh(world, AABB.first.w + j);
        VAOs.emplace(std::make_pair(iteration, AABB.first.w + j), std::make_pair(cube_vaos[j], mesh_vectors.first.size()));
        VBOs.push_back(cube_vbos[j]);
        EBOs.push_back(cube_ebos[j]);
        glBindVertexArray(cube_vaos[j]);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube_ebos[j]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh_vectors.first.size() * sizeof(int), mesh_vectors.first.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, cube_vbos[j]);
        glBufferData(GL_ARRAY_BUFFER, mesh_vectors.second.size() * sizeof(float), mesh_vectors.second.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
        glEnableVertexAttribArray(0);
    }
    delete[] cube_ebos;
    delete[] cube_vbos;
    delete[] cube_vaos;
}

int find(std::vector<vertex> vec, vertex search){
    int lower = 0, upper = vec.size();
    int i = (lower + upper) / 2;
    while(i >= 0 && i < vec.size() && vec[i] != search){
        if(search < vec[i]){
            upper = i;
            i = (lower + upper) / 2;
        } else if(search > vec[i]) {
            lower = i;
            i = (lower + upper) / 2 + 1;
        } else {
            for(int j = lower; j < upper; j++){
                if(vec[j] == search){
                    return j;
                }
            }
            return -1;
        }
    }
    return i;
}

std::pair<std::vector<int>, std::vector<float>> get_mesh(std::unordered_map<coordinate, bool> world, int w){
    std::list<vertex> vertices;

    std::array<vertex, 8> offsets = {
        vertex{-0.45f, -0.45f, -0.45f}, vertex{0.45f, -0.45f, -0.45f}, vertex{0.45f, -0.45f, 0.45f}, vertex{-0.45f, -0.45f, 0.45f}, 
        vertex{-0.45f,  0.45f, -0.45f}, vertex{0.45f,  0.45f, -0.45f}, vertex{0.45f,  0.45f, 0.45f}, vertex{-0.45f,  0.45f, 0.45f}
    };
    for(auto it = world.begin(); it != world.end(); it++){
        if(it->first.w != w){
            continue;
        }
        for(int i = 0; i < offsets.size(); i++){
            vertex value = offsets[i] + it->first;
            auto vit = vertices.begin();
            for(vit; vit != vertices.end() && *vit < value; vit++);
            vertices.insert(vit, value);
        }
    }
    std::vector<vertex> vertices_vec;
    while(vertices.size() > 0){
        vertex elem = vertices.front();
        vertices_vec.push_back(elem);
        vertices.remove(elem);
    }
    std::vector<int> index_data;
    std::vector<float> vertex_data;
    int i = 0;
    for(auto it = vertices_vec.begin(); it != vertices_vec.end(); it++){
        vertex_data.push_back(it->x);
        vertex_data.push_back(it->y);
        vertex_data.push_back(it->z);
        i++;
    }
    for(auto it = world.begin(); it != world.end(); it++){
        if(it->first.w != w){
            continue;
        }
        try {
            //Bottom face
            std::array<int, 8> indices;
            for(int i = 0; i < 8; i++) {
                indices[i] = find(vertices_vec, offsets[i] + it->first);
            }
            index_data.push_back(indices[0]);
            index_data.push_back(indices[1]);
            index_data.push_back(indices[2]);

            index_data.push_back(indices[2]);
            index_data.push_back(indices[3]);
            index_data.push_back(indices[0]);


            //Left face
            index_data.push_back(indices[0]);
            index_data.push_back(indices[3]);
            index_data.push_back(indices[4]);

            index_data.push_back(indices[4]);
            index_data.push_back(indices[3]);
            index_data.push_back(indices[7]);


            //Back face
            index_data.push_back(indices[0]);
            index_data.push_back(indices[4]);
            index_data.push_back(indices[1]);

            index_data.push_back(indices[1]);
            index_data.push_back(indices[4]);
            index_data.push_back(indices[5]);


            //Right face
            index_data.push_back(indices[1]);
            index_data.push_back(indices[5]);
            index_data.push_back(indices[2]);

            index_data.push_back(indices[2]);
            index_data.push_back(indices[5]);
            index_data.push_back(indices[6]);


            //Front face
            index_data.push_back(indices[2]);
            index_data.push_back(indices[6]);
            index_data.push_back(indices[3]);

            index_data.push_back(indices[3]);
            index_data.push_back(indices[6]);
            index_data.push_back(indices[7]);


            //Top face
            index_data.push_back(indices[4]);
            index_data.push_back(indices[7]);
            index_data.push_back(indices[5]);

            index_data.push_back(indices[5]);
            index_data.push_back(indices[7]);
            index_data.push_back(indices[6]);
        }
        catch(std::out_of_range e){
            std::cerr << "Index map out of range: " << it->first << std::endl;
            exit(1);
        }
    }

    return std::make_pair(index_data, vertex_data);
}

int max_cycle = 6;

int draw_conway(rjs::window& win, std::unordered_map<std::pair<int, int>, std::pair<uint, int>> VAOs, int cycles){
    max_cycle = cycles;
    rjs::shader shader("./shaders/conway.vs.glsl", "./shaders/conway.fs.glsl");
    if(!shader.good()){
        return 1;
    }
    shader.use();
    glm::mat4 projection(1.0f);
    //projection = glm::ortho(-12.0f, 12.0f, -8.0f, 8.0f, 0.1f, 1000.0f); 
    projection = glm::perspective(glm::radians(74.0f), (float)win.width() / win.height(), 0.1f, 1000.0f);
    view = glm::translate(view, glm::vec3(0.0f, -5.0f, -10.0f));

    shader.setMat4("model", model);
    shader.setMat4("view", view);
    shader.setMat4("projection", projection);
    
    glClearColor(0.1, 0.1, 0.1, 1.0);

    win.on_draw([VAOs, &shader](double deltaTime){
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        auto VAO_it = VAOs.find({step, currW});
        while(VAO_it == VAOs.end()){
            std::cout << step << ", " << currW << " not found in VAOs" << std::endl;
            if(currW > 0) {
                currW--;
            } else if(currW < 0) {
                currW++;
            } else if(step > 0) {
                step--;
            } else {
                step++;
            }
            VAO_it = VAOs.find({step, currW});
        }
        std::pair<uint, int> VAO = VAO_it->second;
        if(keys_down[GLFW_KEY_LEFT_SHIFT] || keys_down[GLFW_KEY_RIGHT_SHIFT]){
            deltaTime *= 5;
        }

        if(keys_down[GLFW_KEY_W]){
            view = glm::translate(view, (float)deltaTime * 5 * glm::vec3(0.0f, 0.0f, 1.0f));
        }
        if(keys_down[GLFW_KEY_A]){
            view = glm::translate(view, (float)deltaTime * 5 * glm::vec3(1.0f, 0.0f, 0.0f));
        }
        if(keys_down[GLFW_KEY_S]){
            view = glm::translate(view, (float)deltaTime * 5 * glm::vec3(0.0f, 0.0f, -1.0f));
        }
        if(keys_down[GLFW_KEY_D]){
            view = glm::translate(view, (float)deltaTime * 5 * glm::vec3(-1.0f, 0.0f, 0.0f));
        }
        if(keys_down[GLFW_KEY_Q]){
            view = glm::translate(view, (float)deltaTime * 5 * glm::vec3(0.0f, -1.0f, 0.0f));
        }
        if(keys_down[GLFW_KEY_E]){
            view = glm::translate(view, (float)deltaTime * 5 * glm::vec3(0.0f, 1.0f, 0.0f));
        }
        if(keys_down[GLFW_KEY_I]){
            roty += deltaTime * glm::radians(30.0f);
        }
        if(keys_down[GLFW_KEY_J]){
            rotx -= deltaTime * glm::radians(30.0f);
        }
        if(keys_down[GLFW_KEY_K]){
            roty -= deltaTime * glm::radians(30.0f);
        }
        if(keys_down[GLFW_KEY_L]){
            rotx += deltaTime * glm::radians(30.0f);
        }
        model = glm::identity<glm::mat4>();
        model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, (float)rotx, glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, (float)roty, glm::vec3(1.0f, 0.0f, 0.0f));
        shader.setMat4("view", view);
        shader.setMat4("model", model);
        glBindVertexArray(VAO.first);
        glDrawElements(GL_TRIANGLES, VAO.second, GL_UNSIGNED_INT, nullptr);
    });

    win.on_input([](GLFWwindow* glWindow, int key, int scancode, int action, int modifiers){
        keys_down[key] = action != GLFW_RELEASE;
        if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS){
            glfwSetWindowShouldClose(glWindow, true);
        }
        if(key == GLFW_KEY_RIGHT && (action == GLFW_PRESS || action == GLFW_REPEAT)){
            step = (step + 1) % max_cycle;
        }

        if(key == GLFW_KEY_LEFT && (action == GLFW_PRESS || action == GLFW_REPEAT)){
            step = (step - 1 + max_cycle) % max_cycle;
        }

        if(key == GLFW_KEY_UP && (action == GLFW_PRESS || action == GLFW_REPEAT)){
            currW = (currW + 1);
        }

        if(key == GLFW_KEY_DOWN && (action == GLFW_PRESS || action == GLFW_REPEAT)){
            currW = (currW - 1);
        }
    });

    win.on_mouse_button([](GLFWwindow* glWindow, int button, int action, int modifiers){
        keys_down[button] = action != GLFW_RELEASE;
    });

    win.run();
    return 0;
}

#endif