#ifndef VERTEX_HPP
#define VERTEX_HPP

#include <ostream>
#include <iomanip>

namespace rjs {
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

        vertex operator+(const vertex& other) const {
            return {x + other.x, y + other.y, z + other.z};
        }
    };
}

namespace std {
    ostream& operator<<(ostream& os, const rjs::vertex& vert){
        os << "(" << std::setw(4) << std::right << vert.x 
           << "," << std::setw(4) << std::right << vert.y 
           << "," << std::setw(4) << std::right << vert.z << ")";
        return os;
    }
}

#endif