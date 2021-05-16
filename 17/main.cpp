#include <iostream>
#include <fstream>
#include <iomanip>
#include <array>
#include <unordered_map>

struct coordinate {
    int x, y, z;
    int& operator[](int index) {
        if(index == 0)
            return x;
        if(index == 1)
            return y;
        if(index == 2)
            return z;
        throw new std::out_of_range("Must be in range [0, 2]"); 
    }
    bool operator==(const coordinate &other) const {
        return this->x == other.x && this->y == other.y && this->z == other.z;
    }

    /**
     * operator< - Strict compare, returns true iff all three coordinates are <= the other's
     * */
    bool operator<(const coordinate &other) const {
        return this->x <= other.x && this->y <= other.y && this->z <= other.z;
    }

    /**
     * operator> - Strict compare, returns true iff all three coordinates are >= the other's
     * */
    bool operator>(const coordinate &other) const {
        return this->x >= other.x && this->y >= other.y && this->z >= other.z;
    }

    /**
     * operator<= - Loose compare, returns true if any of the coordinates are <= the other's corresponding coordinates
     * */
    bool operator<=(const coordinate &other) const {
        return this->x <= other.x || this->y <= other.y || this->z <= other.z;
    }

    /**
     * operator>= - Loose compare, returns true if any of the coordinates are >= the other's corresponding coordinates
     * */
    bool operator>=(const coordinate &other) const {
        return this->x >= other.x || this->y >= other.y || this->z >= other.z;
    }
};

namespace std {
    template <>
    struct hash<coordinate> {
        size_t operator()(const coordinate& k) const {
            return (size_t)k.x ^ ((size_t)k.y) << 8 ^ ((size_t)k.z) << 16;
        }
    };

    ostream& operator<<(ostream& os, const coordinate& coord){
        os << "(" << std::setw(2) << std::right << coord.x 
           << "," << std::setw(2) << std::right << coord.y 
           << "," << std::setw(2) << std::right << coord.z << ")";
        return os;
    }
}

void print_coord(coordinate c){
    std::cout << "(" << c.x << ", " << c.y << ", " << c.z << ")";
}

int neighbor_count(std::unordered_map<coordinate, bool> world, coordinate cell){
    int count = 0;
    for(int z = cell.z - 1; z <= cell.z + 1; z++){
        for(int y = cell.y - 1; y <= cell.y + 1; y++){
            for(int x = cell.x - 1; x <= cell.x + 1; x++){
                if(x == cell.x && y == cell.y && z == cell.z)
                    continue;
                if(world[{x, y, z}]){
                    //std::cout << "Found neighbor at ";
                    //print_coord({x, y, z});
                    //std::cout << std::endl;
                    count++;
                }
            }
        }
    }
    return count;
}

void draw_plane(std::unordered_map<coordinate, bool> world, int z, std::pair<int, int> x, std::pair<int, int> y){
    std::cout << "   ";
    for(int i = x.first; i <= x.second; i++){
        std::cout << std::setw(3) << std::right << i;
    }
    std::cout << std::endl;
    
    for(int j = y.first; j <= y.second; j++){
        std::cout << std::setw(3) << std::right << j;
        for(int i = x.first; i <= x.second; i++){
            if(world[{i, j, z}]){
                std::cout << "\033[1;32m" << std::setw(3) << std::right << neighbor_count(world, {i, j, z}) << "\033[0m";
            } else {
                std::cout << "\033[1;31m" << std::setw(3) << std::right << neighbor_count(world, {i, j, z}) << "\033[0m";
            }
        }
        std::cout << std::endl;
    }
    std::cout << std::endl << std::endl;
}

void draw_cube(std::unordered_map<coordinate, bool> world, std::pair<coordinate, coordinate> AABB, int offset = 0){
    for(int z = AABB.first.z - offset; z <= AABB.second.z + offset; z++){
        std::cout << "z: " << z << std::endl;
        draw_plane(world, z, std::make_pair(AABB.first.x - offset, AABB.second.x + offset), 
                             std::make_pair(AABB.first.y - offset, AABB.second.y + offset));
    }
}

int find_min_plane(std::unordered_map<coordinate, bool> world,
                    std::pair<int, int> search_bounds,
                    std::pair<int, int> plane1_bounds,
                    std::pair<int, int> plane2_bounds,
                    std::array<int, 3> index,
                    int* cells = nullptr){
    int directionz = (search_bounds.second - search_bounds.first) / std::abs(search_bounds.second - search_bounds.first);
    int directiony = (plane1_bounds.second - plane1_bounds.first) / std::abs(plane1_bounds.second - plane1_bounds.first);
    int directionx = (plane2_bounds.second - plane2_bounds.first) / std::abs(plane2_bounds.second - plane2_bounds.first);
    if(cells != nullptr)
        *cells = 0;
    for(int z = search_bounds.first; z != search_bounds.second + directionz; z+=directionz){
        for(int y = plane1_bounds.first; y != plane1_bounds.second + directiony; y+=directiony){
            for(int x = plane2_bounds.first; x != plane2_bounds.second + directionx; x+=directionx){
                coordinate loc;
                loc[index[0]] = z;
                loc[index[1]] = y;
                loc[index[2]] = x;
                if(cells != nullptr)
                    (*cells)++;
                if(world[loc]){
                    return z;
                }
            }
        }
    }
    return search_bounds.second + directionz;
}

std::pair<coordinate, coordinate> get_minimum_AABB(std::unordered_map<coordinate, bool> world, std::pair<coordinate, coordinate> prevAABB){
    coordinate min = prevAABB.second;
    coordinate max = prevAABB.first;
    if(world.size() < 250){
        for(auto it = world.begin(); it != world.end(); it++){
            if(!it->second)
                continue;
            if(it->first.x < min.x){
                min.x = it->first.x;
            }
            if(it->first.x > max.x){
                max.x = it->first.x;
            }
            if(it->first.y < min.y){
                min.y = it->first.y;
            }
            if(it->first.y > max.y){
                max.y = it->first.y;
            }
            if(it->first.z < min.z){
                min.z = it->first.z;
            }
            if(it->first.z > max.z){
                max.z = it->first.z;
            }
        }
    } else {
        int total_cells = 0, cells;
        min.z = find_min_plane(world, {prevAABB.first.z - 1, prevAABB.second.z}, 
                                    {prevAABB.first.y - 1, prevAABB.second.y + 1}, 
                                    {prevAABB.first.x - 1, prevAABB.second.x + 1},
                                    std::array<int, 3> {2, 1, 0}, &cells);
        total_cells += cells;
        min.y = find_min_plane(world, {prevAABB.first.y - 1, prevAABB.second.y}, 
                                    {prevAABB.first.z - 1, prevAABB.second.z + 1}, 
                                    {prevAABB.first.x - 1, prevAABB.second.x + 1},
                                    std::array<int, 3> {1, 2, 0}, &cells);
        total_cells += cells;
        min.x = find_min_plane(world, {prevAABB.first.x - 1, prevAABB.second.x}, 
                                    {prevAABB.first.y - 1, prevAABB.second.y + 1}, 
                                    {prevAABB.first.z - 1, prevAABB.second.z + 1},
                                    std::array<int, 3> {0, 1, 2}, &cells);
        total_cells += cells;
        max.z = find_min_plane(world, {prevAABB.second.z + 1, prevAABB.first.z}, 
                                    {prevAABB.first.y - 1, prevAABB.second.y + 1}, 
                                    {prevAABB.first.x - 1, prevAABB.second.x + 1},
                                    std::array<int, 3> {2, 1, 0}, &cells);
        total_cells += cells;
        max.y = find_min_plane(world, {prevAABB.second.y + 1, prevAABB.first.y}, 
                                    {prevAABB.first.z - 1, prevAABB.second.z + 1}, 
                                    {prevAABB.first.x - 1, prevAABB.second.x + 1},
                                    std::array<int, 3> {1, 2, 0}, &cells);
        total_cells += cells;
        max.x = find_min_plane(world, {prevAABB.second.x + 1, prevAABB.first.x}, 
                                    {prevAABB.first.y - 1, prevAABB.second.y + 1}, 
                                    {prevAABB.first.z - 1, prevAABB.second.z + 1},
                                    std::array<int, 3> {0, 1, 2}, &cells);
        total_cells += cells;
        std::cout << "Total cells: " << total_cells << std::endl;
        std::cout << "World size:  " << world.size() << std::endl;
    }
    return std::make_pair(min, max);
}

int main(){
    std::ifstream input;
    input.open("input.txt", std::ifstream::in);

    std::unordered_map<coordinate, bool> world1, world2;
    std::string line;
    std::pair<coordinate, coordinate> AABB = std::make_pair(coordinate{0, 0, 0}, coordinate{0, 0, 0}), AABB2 = std::make_pair(coordinate{0, 0, 0}, coordinate{0, 0, 0});
    int x = 0, y = 0, z = 0;
    while(input.good()){
        std::getline(input, line);
        if(line == "")
            break;
        for(int x = 0; x < line.size(); x++){
            switch(line[x]){
            case '#':
                if(coordinate{x, y, z} < AABB.first){
                    AABB.first = coordinate{x, y, z};
                    AABB2.first = AABB.first;
                }
                if(coordinate{x, y, z} > AABB.second){
                    AABB.second = coordinate{x, y, z};
                    AABB2.second = AABB.second;
                }
                world1[{x, y, z}] = true;
                world2[{x, y, z}] = true;
                break;
            case '.':
                world1[{x, y, z}] = false;
                world2[{x, y, z}] = false;
                break;
            }
        }
        y++;
    }
    
    std::unordered_map<coordinate, bool>* world = &world1, *next_world = &world2;
    for(int i = 0; i < 6; i++){
        //std::cout << "Iteration " << i << std::endl;
        AABB = get_minimum_AABB(*world, AABB);
        for(z = AABB.first.z - 1; z <= AABB.second.z + 1; z++){
            for(y = AABB.first.y - 1; y <= AABB.second.y + 1; y++){
                for(x = AABB.first.x - 1; x <= AABB.second.x + 1; x++){
                    //std::cout << coordinate{x, y, z} << ": ";
                    switch(neighbor_count(*world, {x, y, z})){
                    case 2:
                        //std::cout << "maintaining";
                        (*next_world)[{x, y, z}] = (*world)[{x, y, z}];
                        break;
                    case 3:
                        (*next_world)[{x, y, z}] = true;
                        if(!(*world)[{x, y, z}]){
                            //std::cout << "activating";
                        }
                        break;
                    default:
                        (*next_world)[{x, y, z}] = false;
                        if((*world)[{x, y, z}]){
                            //std::cout << "deactivating";
                        }
                        break;
                    }
                    //std::cout << std::endl;
                }
            }
        }
        auto* temp = world;
        world = next_world;
        next_world = temp;
        //break;
        //std::cin.get();
    }
    AABB = get_minimum_AABB(*world, AABB);
    //std::cout << "Iteration " << 6 << std::endl;
    //draw_cube(*world, AABB);
    std::cout << AABB.first << std::endl;
    std::cout << AABB.second << std::endl;
    int active = 0;
    std::cout << "The world has " << world->size() << " cells after 6 cycles" << std::endl;
    for(auto it = world->begin(); it != world->end(); it++){
        if(it->second){
            active++;
        }
    }
    std::cout << "There are " << active << " active cells after 6 cycles" << std::endl;
    return 0;
}