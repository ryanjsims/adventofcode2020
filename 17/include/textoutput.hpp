#ifndef TEXTOUTPUT_HPP
#define TEXTOUTPUT_HPP

#include <iostream>
#include <iomanip>
#include <unordered_map>

#include "coordinate.hpp"

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

#endif