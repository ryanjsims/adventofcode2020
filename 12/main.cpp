#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>

typedef std::pair<char, int> action;

action parse(std::string line){
   return std::make_pair(line[0], std::stoi(line.substr(1)));
}

int main(){
    std::ifstream input;
    input.open("input.txt", std::ifstream::in);

    int x = 0, y = 0, direction = 0;
    int wx = 10, wy = 1, nwx;
    int commands_followed = 0;
    std::string line;
    std::getline(input, line);
    while(input.good()){
        action movement = parse(line);
        switch(movement.first){
        case 'N':
            wy += movement.second;
            //std::cout << "Moved waypoint north " << movement.second << " units" << std::endl;
            break;
        case 'E':
            wx += movement.second;
            //std::cout << "Moved waypoint east  " << movement.second << " units" << std::endl;
            break;
        case 'S':
            wy -= movement.second;
            //std::cout << "Moved waypoint south " << movement.second << " units" << std::endl;
            break;
        case 'W':
            wx -= movement.second;
            //std::cout << "Moved waypoint west  " << movement.second << " units" << std::endl;
            break;
        case 'F':
            //std::cout << "Moved to waypoint " << movement.second << " times" << std::endl;
            x += wx * movement.second;
            y += wy * movement.second;
            break;
        case 'R':
            movement.second = movement.second * -1 + 360;
        case 'L':
            //std::cout << "Rotating waypoint by " << movement.second << " degrees from " << wx << ", " << wy;
            switch (movement.second)
            {
            case 90:
                nwx = -wy;
                wy = wx;
                wx = nwx;
                break;
            case 180:
                wx = -wx;
                wy = -wy;
                break;
            case 270:
                nwx = wy;
                wy = -wx;
                wx = nwx;
                break;
            default:
                break;
            }
            //std::cout << " to " << wx << ", " << wy << std::endl;
            break;
        }
        //std::cout << "Ship is at " << x << ", " << y << " with waypoint at " << wx << ", " << wy << " units away" << std::endl;
        commands_followed++;
        std::getline(input, line);
    }

    std::cout << "The boat is at " << x << ", " << y << ": " << abs(x) + abs(y) << " units from the start" << std::endl;
    std::cout << commands_followed << " commands issued" << std::endl;
    return 0;
}