#include <iostream>
#include <fstream>
#include <iomanip>
#include <array>
#include <unordered_map>
#include <vector>
#include <list>
#include <set>
#include <math.h>

#include "coordinate.hpp"
#include "vertex.hpp"
#include "drawing.hpp"
#include "textoutput.hpp"

#include "../opengl/window.hpp"
#include "../opengl/shader.hpp"

std::pair<coordinate, coordinate> get_minimum_AABB(std::unordered_map<coordinate, bool> world){
    coordinate min, max;
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

void handle_commandline(int argc, char** argv, std::ifstream& input){
    if(argc == 3){
        coordinate::ndim = atoi(argv[2]);
        input.open(argv[1], std::ifstream::in);
    } else if(argc == 2){
        input.open(argv[1], std::ifstream::in);
    } else {
        input.open("input.txt", std::ifstream::in);
    }
}

void read_input(std::ifstream& input, std::unordered_map<coordinate, bool>& world){
    int x = 0, y = 0, z = 0, w = 0;
    std::string line;
    while(input.good()){
        std::getline(input, line);
        if(line == "")
            break;
        for(x = 0; x < line.size(); x++){
            coordinate curr = {x, y, z, w};
            switch(line[x]){
            case '#':
                world[curr] = true;
                break;
            }
        }
        y++;
    }
}


int main(int argc, char** argv){
    rjs::window win("Conway Multidimensional Visualizer", 100, 100, 800, 600);
    std::ifstream input;
    handle_commandline(argc, argv, input);

    std::unordered_map<coordinate, bool> world1, world2;
    read_input(input, world1);
    std::pair<coordinate, coordinate> AABB = get_minimum_AABB(world1);    
    
    std::unordered_map<coordinate, bool>* world = &world1, *next_world = &world2;
    int i;
    std::vector<uint> VBOs;
    std::vector<uint> EBOs;
    //Mapping from (life step, w coord) to (model, num vertices)
    std::unordered_map<std::pair<int, int>, std::pair<uint, int>> VAOs;

    for(i = 0; i < 6; i++){
        //std::cout << "Iteration " << i << std::endl;
        std::unordered_map<coordinate, int> frontier = generate_frontier(*world);
        AABB = get_minimum_AABB(*world);
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
    //AABB = get_minimum_AABB(*world, AABB);
    //std::unordered_map<coordinate, int> frontier = generate_frontier(*world);
    //fdraw_hypercube(frontier, *world, AABB);
    //std::cout << AABB.first << std::endl;
    //std::cout << AABB.second << std::endl;
    int active = 0;
    std::cout << "There are " << world->size() << " active cells after " << i << " cycles" << std::endl;
    std::cout << "There are " << next_world->size() << " active cells after " << i - 1 << " cycles" << std::endl;

    draw_conway(win, VAOs);    
    return 0;
}