#include <iostream>
#include <fstream>
#include <iomanip>
#include <array>
#include <unordered_map>
#include <vector>
#include <math.h>

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

namespace std {
    template <>
    struct hash<coordinate> {
        size_t operator()(const coordinate& k) const {
            return (size_t)k.x ^ ((size_t)k.y) << 8 ^ ((size_t)k.z) << 16 ^ ((size_t)k.w) << 24;
        }
    };

    ostream& operator<<(ostream& os, const coordinate& coord){
        os << "(" << std::setw(2) << std::right << coord.x 
           << "," << std::setw(2) << std::right << coord.y 
           << "," << std::setw(2) << std::right << coord.z
           << "," << std::setw(2) << std::right << coord.w << ")";
        return os;
    }
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
                std::cout << "\033[1;32m" << std::setw(3) << std::right << frontier[curr] << "\033[0m";
            } else {
                std::cout << "\033[1;31m" << std::setw(3) << std::right << frontier[curr] << "\033[0m";
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
        if(min.w != 0 || max.w != 0){
            std::cout << "get_minimum_AABB\n" << min << std::endl << max << std::endl;
        }
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
    bool p = false;
    for(int i = 0; i < neighbors.size(); i++){
        if(neighbors[i].w != 0)
            p = true;
    }
    if(p){
        for(int i = 0; i < neighbors.size(); i++){
            std::cout << neighbors[i] << std::endl;
        }
    }
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

int main(int argc, char** argv){
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
    for(i = 0; i < 6; i++){
        std::cout << "Iteration " << i << std::endl;
        std::unordered_map<coordinate, int> frontier = generate_frontier(*world);
        AABB = get_minimum_AABB(*world, AABB);
        if(AABB.first.w != 0 || AABB.second.w != 0){
            std::cout << AABB.first << std::endl;
            std::cout << AABB.second << std::endl;
        }
        fdraw_cube(frontier, *world, 0, AABB);
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
    std::cout << "Iteration " << i << std::endl;
    AABB = get_minimum_AABB(*world, AABB);
    std::unordered_map<coordinate, int> frontier = generate_frontier(*world);
    fdraw_cube(frontier, *world, 0, AABB);
    std::cout << AABB.first << std::endl;
    std::cout << AABB.second << std::endl;
    int active = 0;
    std::cout << "There are " << world->size() << " active cells after " << i << " cycles" << std::endl;
    std::cout << "There are " << next_world->size() << " active cells after " << i - 1 << " cycles" << std::endl;
    return 0;
}