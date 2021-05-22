#ifndef COORDINATE_HPP
#define COORDINATE_HPP

#include <cstddef>
#include <ostream>
#include <iomanip>
#include <algorithm>
#include <stdexcept>

#include <iostream>

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
        x = other.x;
        y = other.y;
        z = other.z;
        w = other.w;
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

#endif