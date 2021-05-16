#include <iostream>
#include <fstream>
#include <iomanip>
#include <array>
#include <unordered_map>

struct coordinate {
    int x, y, z, w;
    int& operator[](int index) {
        if(index == 0)
            return x;
        if(index == 1)
            return y;
        if(index == 2)
            return z;
        if(index == 3)
            return w;
        throw new std::out_of_range("Must be in range [0, 2]"); 
    }

    inline std::size_t size(){
        return 4;
    }

    bool operator==(const coordinate &other) const {
        return this->x == other.x && this->y == other.y && this->z == other.z && this->w == other.w;
    }

    /**
     * operator< - Strict compare, returns true iff all three coordinates are <= the other's
     * */
    bool operator<(const coordinate &other) const {
        return this->x <= other.x && this->y <= other.y && this->z <= other.z && this->w <= other.w;
    }

    /**
     * operator> - Strict compare, returns true iff all three coordinates are >= the other's
     * */
    bool operator>(const coordinate &other) const {
        return this->x >= other.x && this->y >= other.y && this->z >= other.z && this->w >= other.w;
    }

    /**
     * operator<= - Loose compare, returns true if any of the coordinates are <= the other's corresponding coordinates
     * */
    bool operator<=(const coordinate &other) const {
        return this->x <= other.x || this->y <= other.y || this->z <= other.z || this->w <= other.w;
    }

    /**
     * operator>= - Loose compare, returns true if any of the coordinates are >= the other's corresponding coordinates
     * */
    bool operator>=(const coordinate &other) const {
        return this->x >= other.x || this->y >= other.y || this->z >= other.z || this->w >= other.w;
    }

    coordinate minimize(coordinate other){
        coordinate to_return;
        for(int i = 0; i < to_return.size(); i++){
            to_return[i] = std::min((*this)[i], other[i]);
        }
        return to_return;
    }

    coordinate maximize(coordinate other){
        coordinate to_return;
        for(int i = 0; i < to_return.size(); i++){
            to_return[i] = std::max((*this)[i], other[i]);
        }
        return to_return;
    }
};

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

int neighbor_count(std::unordered_map<coordinate, bool> world, coordinate cell){
    int count = 0;
    for(int w = cell.w - 1; w <= cell.w + 1; w++){
        for(int z = cell.z - 1; z <= cell.z + 1; z++){
            for(int y = cell.y - 1; y <= cell.y + 1; y++){
                for(int x = cell.x - 1; x <= cell.x + 1; x++){
                    if(coordinate{x, y, z, w} == cell)
                        continue;
                    if(world[{x, y, z, w}]){
                        //std::cout << "Found neighbor at ";
                        //print_coord({x, y, z});
                        //std::cout << std::endl;
                        count++;
                    }
                }
            }
        }
    }
    return count;
}

void draw_plane(std::unordered_map<coordinate, bool> world, int w, int z, std::pair<int, int> x, std::pair<int, int> y){
    std::cout << "   ";
    for(int i = x.first; i <= x.second; i++){
        std::cout << std::setw(3) << std::right << i;
    }
    std::cout << std::endl;
    
    for(int j = y.first; j <= y.second; j++){
        std::cout << std::setw(3) << std::right << j;
        for(int i = x.first; i <= x.second; i++){
            if(world[{i, j, z}]){
                std::cout << "\033[1;32m" << std::setw(3) << std::right << neighbor_count(world, {i, j, z, w}) << "\033[0m";
            } else {
                std::cout << "\033[1;31m" << std::setw(3) << std::right << neighbor_count(world, {i, j, z, w}) << "\033[0m";
            }
        }
        std::cout << std::endl;
    }
    std::cout << std::endl << std::endl;
}

void draw_cube(std::unordered_map<coordinate, bool> world, int w, std::pair<coordinate, coordinate> AABB, int offset = 0){
    for(int z = AABB.first.z - offset; z <= AABB.second.z + offset; z++){
        std::cout << "(z, w): (" << z << ", " << w << ")" << std::endl;
        draw_plane(world, w, z, std::make_pair(AABB.first.x - offset, AABB.second.x + offset), 
                             std::make_pair(AABB.first.y - offset, AABB.second.y + offset));
    }
}

void draw_hypercube(std::unordered_map<coordinate, bool> world, std::pair<coordinate, coordinate> AABB, int offset = 0){
    for(int w = AABB.first.w - offset; w <= AABB.second.w + offset; w++){
        draw_cube(world, w, AABB, offset);
    }
}

int find_min_cube(std::unordered_map<coordinate, bool> world,
                    std::pair<int, int> search_bounds,
                    std::pair<int, int> plane1_bounds,
                    std::pair<int, int> plane2_bounds,
                    std::pair<int, int> plane3_bounds,
                    std::array<int, 4> index,
                    int* cells = nullptr){
    int directionz = (search_bounds.second - search_bounds.first) / std::abs(search_bounds.second - search_bounds.first);
    int directiony = (plane1_bounds.second - plane1_bounds.first) / std::abs(plane1_bounds.second - plane1_bounds.first);
    int directionx = (plane2_bounds.second - plane2_bounds.first) / std::abs(plane2_bounds.second - plane2_bounds.first);
    int directionw = (plane3_bounds.second - plane3_bounds.first) / std::abs(plane3_bounds.second - plane3_bounds.first);
    if(cells != nullptr)
        *cells = 0;
    for(int z = search_bounds.first; z != search_bounds.second + directionz; z+=directionz){
        for(int y = plane1_bounds.first; y != plane1_bounds.second + directiony; y+=directiony){
            for(int x = plane2_bounds.first; x != plane2_bounds.second + directionx; x+=directionx){
                for(int w = plane3_bounds.first; w != plane3_bounds.second + directionw; w+=directionw){
                    coordinate loc;
                    loc[index[0]] = z;
                    loc[index[1]] = y;
                    loc[index[2]] = x;
                    loc[index[3]] = w;
                    if(cells != nullptr)
                        (*cells)++;
                    if(world[loc]){
                        return z;
                    }
                }
            }
        }
    }
    return search_bounds.second + directionz;
}

std::pair<coordinate, coordinate> get_minimum_AABB(std::unordered_map<coordinate, bool> world, std::pair<coordinate, coordinate> prevAABB){
    coordinate min = {prevAABB.second.x, prevAABB.second.y, prevAABB.second.z, prevAABB.second.w};
    coordinate max = {prevAABB.first.x, prevAABB.first.y, prevAABB.first.z, prevAABB.first.w};
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
        if(it->first.w < min.w){
            min.w = it->first.w;
        }
        if(it->first.w > max.w){
            max.w = it->first.w;
        }
    }
    return std::make_pair(min, max);
}

int main(){
    std::ifstream input;
    input.open("smallinput.txt", std::ifstream::in);

    std::unordered_map<coordinate, bool> world1, world2;
    std::string line;
    std::pair<coordinate, coordinate> AABB = std::make_pair(coordinate{0, 0, 0, 0}, coordinate{0, 0, 0, 0});
    int x = 0, y = 0, z = 0, w = 0;
    while(input.good()){
        std::getline(input, line);
        std::cout << line << std::endl;
        if(line == "")
            break;
        for(int x = 0; x < line.size(); x++){
            coordinate curr = {x, y, z, w};
            switch(line[x]){
            case '#':
                AABB.first = AABB.first.minimize(curr);
                AABB.second = AABB.second.maximize(curr);
                world1[curr] = true;
                world2[curr] = true;
                break;
            case '.':
                world1[curr] = false;
                world2[curr] = false;
                break;
            }
        }
        y++;
    }
    std::cout << AABB.first << std::endl;
    std::cout << AABB.second << std::endl;
    std::cin.get();
    
    std::unordered_map<coordinate, bool>* world = &world1, *next_world = &world2;
    for(int i = 0; i < 6; i++){
        std::cout << "Iteration " << i << std::endl;
        draw_hypercube(*world, AABB);
        AABB = get_minimum_AABB(*world, AABB);
        for(w = AABB.first.w - 1; w <= AABB.second.w + 1; w++){
            for(z = AABB.first.z - 1; z <= AABB.second.z + 1; z++){
                for(y = AABB.first.y - 1; y <= AABB.second.y + 1; y++){
                    for(x = AABB.first.x - 1; x <= AABB.second.x + 1; x++){
                        //std::cout << coordinate{x, y, z} << ": ";
                        switch(neighbor_count(*world, {x, y, z, w})){
                        case 2:
                            //std::cout << "maintaining";
                            (*next_world)[{x, y, z, w}] = (*world)[{x, y, z, w}];
                            break;
                        case 3:
                            (*next_world)[{x, y, z, w}] = true;
                            if(!(*world)[{x, y, z, w}]){
                                //std::cout << "activating";
                            }
                            break;
                        default:
                            (*next_world)[{x, y, z, w}] = false;
                            if((*world)[{x, y, z, w}]){
                                //std::cout << "deactivating";
                            }
                            break;
                        }
                        //std::cout << std::endl;
                    }
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
    std::cout << "Iteration " << 6 << std::endl;
    draw_hypercube(*world, AABB);
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