#include <iostream>
#include <iomanip>
#include <fstream>
#include <unordered_map>
#include <math.h>
#include <list>
#include <algorithm>

#define vert 0.866
#define horz 0.5

namespace std {
    template <>
    struct hash<pair<double, double>> {
        size_t operator()(const pair<double, double>& k) const {
            return hash<double>()(std::get<0>(k)) ^ (std::hash<double>()(std::get<1>(k)) << 1);
        }
    };

    ostream& operator<<(ostream& os, const pair<double, double>& coord){
        os << "(" << std::setw(13) << std::setprecision(10) << std::right << coord.first
           << "," << std::setw(13) << std::setprecision(10) << std::right << coord.second
           << ")";
        return os;
    }
}

enum class directions {
    east,
    west,
    northwest,
    northeast,
    southwest,
    southeast
};

void print_path(std::list<directions> path){
    for(auto it = path.begin(); it != path.end(); it++){
        switch(*it){
        case directions::east:
            std::cout << "East" << std::endl;
            break;
        case directions::west:
            std::cout << "West" << std::endl;
            break;
        case directions::northeast:
            std::cout << "Northeast" << std::endl;
            break;
        case directions::northwest:
            std::cout << "Northwest" << std::endl;
            break;
        case directions::southeast:
            std::cout << "Southeast" << std::endl;
            break;
        case directions::southwest:
            std::cout << "Southwest" << std::endl;
            break;
        }
    }
}

std::list<directions> tokenize_path(std::string path){
    std::list<directions> output;
    int i = 0;
    while(i < path.size()){
        switch(path[i]){
        case 'w':
            output.push_back(directions::west);
            break;
        case 'e':
            output.push_back(directions::east);
            break;
        case 'n':
            i++;
            switch(path[i]){
            case 'w':
                output.push_back(directions::northwest);
                break;
            case 'e':
                output.push_back(directions::northeast);
                break;
            }
            break;
        case 's':
            i++;
            switch(path[i]){
            case 'w':
                output.push_back(directions::southwest);
                break;
            case 'e':
                output.push_back(directions::southeast);
                break;
            }
            break;
        }
        i++;
    }
    return output;
}

std::list<directions> normalize_path(std::list<directions> path){
    std::list<directions> output;
    std::list<directions>::iterator it;
    while(path.size() != 0){
        switch(path.front()){
        case directions::east:
            if((it = std::find(path.begin(), path.end(), directions::west)) != path.end()){
                path.erase(it);
            } else {
                output.push_back(path.front());
            }
            break;
        case directions::west:
            if((it = std::find(path.begin(), path.end(), directions::east)) != path.end()){
                path.erase(it);
            } else {
                output.push_back(path.front());
            }
            break;
        case directions::northeast:
            if((it = std::find(path.begin(), path.end(), directions::southwest)) != path.end()){
                path.erase(it);
            } else {
                output.push_back(path.front());
            }
            break;
        case directions::northwest:
            if((it = std::find(path.begin(), path.end(), directions::southeast)) != path.end()){
                path.erase(it);
            } else {
                output.push_back(path.front());
            }
            break;
        case directions::southeast:
            if((it = std::find(path.begin(), path.end(), directions::northwest)) != path.end()){
                path.erase(it);
            } else {
                output.push_back(path.front());
            }
            break;
        case directions::southwest:
            if((it = std::find(path.begin(), path.end(), directions::northeast)) != path.end()){
                path.erase(it);
            } else {
                output.push_back(path.front());
            }
            break;

        }
        path.pop_front();
    }
    return output;
}

std::pair<double, double> follow_path(std::list<directions> path){
    double x = 0;
    double y = 0;
    int i = 0;
    while(path.size() > 0){
        int v = -1;
        switch(path.front()){
        case directions::east:
            x += 1;
            break;
        case directions::west:
            x -= 1;
            break;
        case directions::northeast:
            x += horz;
            y += vert;
            break;
        case directions::northwest:
            x -= horz;
            y += vert;
            break;
        case directions::southeast:
            x += horz;
            y -= vert;
            break;
        case directions::southwest:
            x -= horz;
            y -= vert;
            break;
        }
        path.pop_front();
    }
    if(abs(x) < 0.01)
        x = 0;
    if(abs(y) < 0.01)
        y = 0;
    x = round(x * 1000.0) / 1000.0;
    y = round(y * 1000.0) / 1000.0;
    return std::make_pair(x, y);
}

int main(int argc, char** argv){
    std::string filename = "input.txt";
    std::ifstream input;
    if(argc == 2){
        filename = argv[1];
    }
    input.open(filename, std::ifstream::in);

    std::unordered_map<std::pair<double, double>, bool> floor;
    std::string line;
    while(input.good()){
        std::getline(input, line);
        if(line.size() == 0)
            continue;
        std::pair<double, double> coord = follow_path(normalize_path(tokenize_path(line)));
        std::unordered_map<std::pair<double, double>, bool>::iterator it;
        if((it = floor.find(coord)) == floor.end()){
            std::cout << coord << " <new> " << std::endl;
            floor.insert(std::make_pair(coord, false));
        } else {
            std::cout << coord << std::endl;
        }
        floor.at(coord) = !floor.at(coord);

    }
    int i = 0;
    for(auto it = floor.begin(); it != floor.end(); it++){
        if(it->second){
            i++;
        }
    }
    std::cout << "There are " << i << " tiles that were flipped an odd number of times out of " << floor.size() << " tiles" << std::endl;
    return 0;
}