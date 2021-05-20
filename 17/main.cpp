#include <iostream>
#include <fstream>
#include <iomanip>
#include <array>
#include <unordered_map>
#include <vector>
#include <list>
#include <set>
#include <math.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../opengl/window.hpp"
#include "../opengl/shader.hpp"

#define MAX_DIM 4


struct coordinate {
    coordinate(){
        this->x = 0;
        this->y = 0;
        this->z = 0;
        this->w = 0;
    }

    coordinate(int x, int y, int z, int w){
        this->x = x;
        this->y = y;
        this->z = z;
        this->w = w;
    }

    coordinate(const coordinate& other){
        for(int i = 0; i < size(); i++){
            data[i] = other.data[i];
        }
    }
    union{
        struct {
            int x, y, z, w;
        };
        int data[MAX_DIM];
    };
    static size_t ndim;
    
    int& operator[](int index) {
        if(index < 0 || index >= size())
            throw new std::out_of_range("incorrect index"); 
        return data[index];
    }

    inline std::size_t size() const{
        return std::min(ndim, (size_t)MAX_DIM);
    }

    bool operator==(const coordinate &other) const {
        bool to_return = true;
        for(int i = 0; i < size(); i++){
            to_return &= data[i] == other.data[i];
        }
        return to_return;
    }

    /**
     * operator< - Strict compare, returns true iff all three coordinates are <= the other's
     * */
    bool operator<(const coordinate &other) const {
        bool to_return = true;
        for(int i = 0; i < size(); i++){
            to_return &= data[i] <= other.data[i];
        }
        return to_return;;
    }

    /**
     * operator> - Strict compare, returns true iff all three coordinates are >= the other's
     * */
    bool operator>(const coordinate &other) const {
        bool to_return = true;
        for(int i = 0; i < size(); i++){
            to_return &= data[i] >= other.data[i];
        }
        return to_return;
    }

    /**
     * operator<= - Loose compare, returns true if any of the coordinates are <= the other's corresponding coordinates
     * */
    bool operator<=(const coordinate &other) const {
        bool to_return = false;
        for(int i = 0; i < size(); i++){
            to_return |= data[i] <= other.data[i];
        }
        return to_return;
    }

    /**
     * operator>= - Loose compare, returns true if any of the coordinates are >= the other's corresponding coordinates
     * */
    bool operator>=(const coordinate &other) const {
        bool to_return = false;
        for(int i = 0; i < size(); i++){
            to_return |= data[i] >= other.data[i];
        }
        return to_return;
    }

    coordinate minimize(coordinate other){
        coordinate to_return{};
        for(int i = 0; i < to_return.size(); i++){
            to_return[i] = std::min((*this)[i], other[i]);
        }
        return to_return;
    }

    coordinate maximize(coordinate other){
        coordinate to_return{};
        for(int i = 0; i < to_return.size(); i++){
            to_return[i] = std::max((*this)[i], other[i]);
        }
        return to_return;
    }
};

size_t coordinate::ndim = 4;

struct vertex {
    float x, y, z;

    bool operator<(const vertex& other) const {
        return (x * x + y * y + z * z) < (other.x * other.x + other.y * other.y + other.z * other.z);
    }

    bool operator>(const vertex& other) const {
        return (x * x + y * y + z * z) > (other.x * other.x + other.y * other.y + other.z * other.z);
    }

    bool operator==(const vertex& other) const {
        return x == other.x && y == other.y && z == other.z;
    }

    bool operator!=(const vertex& other) const {
        return !(this->operator==(other));
    }

    vertex operator+(const coordinate& rhs) const {
        return {x + rhs.x, y + rhs.y, z + rhs.z};
    }
};

namespace std {
    template <>
    struct hash<coordinate> {
        size_t operator()(const coordinate& k) const {
            return (size_t)k.x ^ ((size_t)k.y) << 8 ^ ((size_t)k.z) << 16 ^ ((size_t)k.w) << 24;
        }
    };

    template <>
    struct hash<pair<int,int>> {
        size_t operator()(const pair<int,int>& k) const {
            return (size_t)k.first ^ (size_t)k.second << 16;
        }
    };

    ostream& operator<<(ostream& os, const coordinate& coord){
        os << "(" << std::setw(2) << std::right << coord.x 
           << "," << std::setw(2) << std::right << coord.y 
           << "," << std::setw(2) << std::right << coord.z
           << "," << std::setw(2) << std::right << coord.w << ")";
        return os;
    }

    ostream& operator<<(ostream& os, const vertex& vert){
        os << "(" << std::setw(4) << std::right << vert.x 
           << "," << std::setw(4) << std::right << vert.y 
           << "," << std::setw(4) << std::right << vert.z << ")";
        return os;
    }
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
        vertex{-0.5f, -0.5f, -0.5f}, vertex{0.5f, -0.5f, -0.5f}, vertex{0.5f, -0.5f, 0.5f}, vertex{-0.5f, -0.5f, 0.5f}, 
        vertex{-0.5f,  0.5f, -0.5f}, vertex{0.5f,  0.5f, -0.5f}, vertex{0.5f,  0.5f, 0.5f}, vertex{-0.5f,  0.5f, 0.5f}
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

void fdraw_plane(std::unordered_map<coordinate, int> frontier, const std::unordered_map<coordinate, bool> world, int w, int z, std::pair<int, int> x, std::pair<int, int> y){
    std::cout << "   ";
    for(int i = x.first; i <= x.second; i++){
        std::cout << std::setw(3) << std::right << i;
    }
    std::cout << std::endl;
    
    for(int j = y.first; j <= y.second; j++){
        std::cout << std::setw(3) << std::right << j;
        for(int i = x.first; i <= x.second; i++){
            coordinate curr = {i, j, z, w};
            if(frontier.find(curr) == frontier.end()){
                std::cout << "\033[1;31m  0\033[0m";
                continue;
            }
            if(world.find(curr) != world.end()) {
                std::cout << "\033[1;32m" << std::setw(3) << std::right << frontier.at(curr) << "\033[0m";
            } else {
                std::cout << "\033[1;31m" << std::setw(3) << std::right << frontier.at(curr) << "\033[0m";
            }
        }
        std::cout << std::endl;
    }
    std::cout << std::endl << std::endl;
}

void fdraw_cube(std::unordered_map<coordinate, int> frontier, const std::unordered_map<coordinate, bool> world, int w, std::pair<coordinate, coordinate> AABB, int offset = 0){
    for(int z = AABB.first.z - offset; z <= AABB.second.z + offset; z++){
        std::cout << "(z, w): (" << z << ", " << w << ")" << std::endl;
        fdraw_plane(frontier, world, w, z, std::make_pair(AABB.first.x - offset, AABB.second.x + offset), 
                             std::make_pair(AABB.first.y - offset, AABB.second.y + offset));
    }
}

void fdraw_hypercube(std::unordered_map<coordinate, int> frontier, const std::unordered_map<coordinate, bool> world, std::pair<coordinate, coordinate> AABB, int offset = 0){
    for(int w = AABB.first.w - offset; w <= AABB.second.w + offset; w++){
        fdraw_cube(frontier, world, w, AABB, offset);
    }
}

std::pair<coordinate, coordinate> get_minimum_AABB(std::unordered_map<coordinate, bool> world, std::pair<coordinate, coordinate> prevAABB){
    coordinate min = {prevAABB.second.x, prevAABB.second.y, prevAABB.second.z, prevAABB.second.w};
    coordinate max = {prevAABB.first.x, prevAABB.first.y, prevAABB.first.z, prevAABB.first.w};
    for(auto it = world.begin(); it != world.end(); it++){
        if(!it->second)
            continue;
        min = min.minimize(it->first);
        max = max.maximize(it->first);
    }
    return std::make_pair(min, max);
}

void generate_neighbors_rec(const coordinate location, int dim, std::vector<coordinate> &neighbors){
    coordinate permutation = location;
    if(dim == location.size()){
        neighbors.push_back(permutation);
        return;
    }
    for(int i = -1; i <= 1; i++){
        permutation.data[dim] = location.data[dim] + i;
        generate_neighbors_rec(permutation, dim + 1, neighbors);
    }
}


std::vector<coordinate> generate_neighbors(coordinate location){
    std::vector<coordinate> neighbors;
    generate_neighbors_rec(location, 0, neighbors);
    return neighbors;
}

std::unordered_map<coordinate, int> generate_frontier(std::unordered_map<coordinate, bool> world){
    std::unordered_map<coordinate, int> frontier;
    for(auto it = world.begin(); it != world.end(); it++){
        std::vector<coordinate> neighbors = generate_neighbors(it->first);
        for(auto nit = neighbors.begin(); nit != neighbors.end(); nit++){
            coordinate curr = *nit;
            if(frontier.find(curr) == frontier.end()){
                frontier[curr] = 0;
            }
            if(!(curr == it->first))
                frontier[curr]++;
        }
    }
    return frontier;
}

int currW = 0, step = 0;
glm::mat4 view(1.0f);
std::unordered_map<int, bool> keys_down;

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
        std::cout << "\nIteration " << iteration << " w: " << AABB.first.w + j << std::endl;
        std::cout << mesh_vectors.first.size() << " " << mesh_vectors.second.size() << std::endl;
        // for(int k = 0; k < mesh_vectors.first.size(); k++){
        //     std::cout << mesh_vectors.second[mesh_vectors.first[k] * 3]
        //                 << ", " << mesh_vectors.second[mesh_vectors.first[k] * 3 + 1]
        //                 << ", " << mesh_vectors.second[mesh_vectors.first[k] * 3 + 2] << std::endl;
        // }
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

int main(int argc, char** argv){
    rjs::window win("Conway Multidimensional Visualizer", 100, 100, 800, 600, 30);
    std::ifstream input;
    if(argc == 3){
        coordinate::ndim = atoi(argv[2]);
        input.open(argv[1], std::ifstream::in);
    } else if(argc == 2){
        input.open(argv[1], std::ifstream::in);
    } else {
        input.open("input.txt", std::ifstream::in);
    }

    std::unordered_map<coordinate, bool> world1, world2;
    std::string line;
    std::pair<coordinate, coordinate> AABB = std::make_pair(coordinate{0, 0, 0, 0}, coordinate{0, 0, 0, 0});
    int x = 0, y = 0, z = 0, w = 0;
    while(input.good()){
        std::getline(input, line);
        if(line == "")
            break;
        for(int x = 0; x < line.size(); x++){
            coordinate curr = {x, y, z, w};
            switch(line[x]){
            case '#':
                AABB.first = AABB.first.minimize(curr);
                AABB.second = AABB.second.maximize(curr);
                world1[curr] = true;
                break;
            }
        }
        y++;
    }
    
    std::unordered_map<coordinate, bool>* world = &world1, *next_world = &world2;
    int i;
    std::vector<uint> VBOs;
    std::vector<uint> EBOs;
    //Mapping from (life step, w coord) to (model, num vertices)
    std::unordered_map<std::pair<int, int>, std::pair<uint, int>> VAOs;

    // uint vao, vbo, ebo;
    // glGenVertexArrays(1, &vao);
    // glGenBuffers(1, &vbo);
    // glGenBuffers(1, &ebo);

    // float vertices[] = {
    //     -0.5, -0.5, -0.5, 1.0f, 0.0f, 0.0f,
    //      0.5, -0.5, -0.5, 0.0f, 1.0f, 0.0f,
    //      0.5, -0.5,  0.5, 0.0f, 0.0f, 1.0f,
    //     -0.5, -0.5,  0.5, 1.0f, 1.0f, 0.0f,
    //     -0.5,  0.5, -0.5, 1.0f, 0.0f, 1.0f,
    //      0.5,  0.5, -0.5, 0.0f, 1.0f, 1.0f,
    //      0.5,  0.5,  0.5, 1.0f, 1.0f, 1.0f,
    //     -0.5,  0.5,  0.5, 0.0f, 0.0f, 0.0f
    // };

    // int indices[] = {
    //     0, 1, 2,
    //     2, 3, 0,

    //     0, 3, 4,
    //     4, 3, 7,

    //     0, 4, 1,
    //     1, 4, 5,

    //     1, 5, 2,
    //     2, 5, 6,

    //     2, 6, 3,
    //     3, 6, 7,

    //     4, 7, 5,
    //     5, 7, 6
    // };

    // VAOs.emplace(std::make_pair(0, 0), std::make_pair(vao, sizeof(indices) / sizeof(int)));
    // glBindVertexArray(vao);
    // glBindBuffer(GL_ARRAY_BUFFER, vbo);
    // glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);
    // glEnableVertexAttribArray(0);
    // glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    // glEnableVertexAttribArray(1);
    // VBOs.push_back(vbo);
    // EBOs.push_back(ebo);

    for(i = 0; i < 6; i++){
        //std::cout << "Iteration " << i << std::endl;
        std::unordered_map<coordinate, int> frontier = generate_frontier(*world);
        AABB = get_minimum_AABB(*world, AABB);
        load_vertices(*world, AABB, i, VAOs, VBOs, EBOs);
        
        //fdraw_hypercube(frontier, *world, AABB, 1);
        next_world->clear();
        for(auto it = frontier.begin(); it != frontier.end(); it++){
            switch(it->second){
                case 2:
                    if(world->find(it->first) != world->end()){
                        next_world->emplace(it->first, true);
                    }
                    break;
                case 3:
                    next_world->emplace(it->first, true);
                    break;
                default:
                    break;
            }
        }
        auto* temp = world;
        world = next_world;
        next_world = temp;
        //break;
        //std::cin.get();
    }
    //std::cout << "Iteration " << i << std::endl;
    AABB = get_minimum_AABB(*world, AABB);
    std::unordered_map<coordinate, int> frontier = generate_frontier(*world);
    //fdraw_hypercube(frontier, *world, AABB);
    std::cout << AABB.first << std::endl;
    std::cout << AABB.second << std::endl;
    int active = 0;
    std::cout << "There are " << world->size() << " active cells after " << i << " cycles" << std::endl;
    std::cout << "There are " << next_world->size() << " active cells after " << i - 1 << " cycles" << std::endl;

    rjs::shader shader("./shaders/conway.vs.glsl", "./shaders/conway.fs.glsl");
    if(!shader.good()){
        return 1;
    }
    shader.use();
    glm::mat4 model(1.0f), projection(1.0f);
    projection = glm::perspective(glm::radians(74.0f), (float)win.width() / win.height(), 1.0f, 1000.0f);
    model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    view = glm::translate(view, glm::vec3(0.0f, -5.0f, -10.0f));

    shader.setMat4("model", model);
    shader.setMat4("view", view);
    shader.setMat4("projection", projection);
    
    glClearColor(0.1, 0.1, 0.1, 1.0);

    win.on_draw([VAOs, &shader](double deltaTime){
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        auto VAO_it = VAOs.find({step, currW});
        if(VAO_it == VAOs.end()){
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
            return;
        }
        std::pair<uint, int> VAO = VAO_it->second;

        if(keys_down[GLFW_KEY_W]){
            view = glm::translate(view, (float)deltaTime * glm::vec3(0.0f, 0.0f, 1.0f));
        }
        if(keys_down[GLFW_KEY_A]){
            view = glm::translate(view, (float)deltaTime * glm::vec3(1.0f, 0.0f, 0.0f));
        }
        if(keys_down[GLFW_KEY_S]){
            view = glm::translate(view, (float)deltaTime * glm::vec3(0.0f, 0.0f, -1.0f));
        }
        if(keys_down[GLFW_KEY_D]){
            view = glm::translate(view, (float)deltaTime * glm::vec3(-1.0f, 0.0f, 0.0f));
        }
        if(keys_down[GLFW_KEY_Q]){
            view = glm::translate(view, (float)deltaTime * glm::vec3(0.0f, -1.0f, 0.0f));
        }
        if(keys_down[GLFW_KEY_E]){
            view = glm::translate(view, (float)deltaTime * glm::vec3(0.0f, 1.0f, 0.0f));
        }
        shader.setMat4("view", view);
        glBindVertexArray(VAO.first);
        glDrawElements(GL_TRIANGLES, VAO.second, GL_UNSIGNED_INT, nullptr);
    });

    win.on_input([](GLFWwindow* glWindow, int key, int scancode, int action, int modifiers){
        if(action == GLFW_PRESS){
            keys_down[key] = true;
        } else if(action == GLFW_RELEASE){
            keys_down[key] = false;
        }
        if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS){
            glfwSetWindowShouldClose(glWindow, true);
        }
        if(key == GLFW_KEY_RIGHT && (action == GLFW_PRESS || action == GLFW_REPEAT)){
            step = (step + 1) % 6;
        }

        if(key == GLFW_KEY_LEFT && (action == GLFW_PRESS || action == GLFW_REPEAT)){
            step = (step - 1) % 6;
        }

        if(key == GLFW_KEY_UP && (action == GLFW_PRESS || action == GLFW_REPEAT)){
            currW = (currW + 1);
        }

        if(key == GLFW_KEY_DOWN && (action == GLFW_PRESS || action == GLFW_REPEAT)){
            currW = (currW - 1);
        }
    });
    win.run();
    return 0;
}