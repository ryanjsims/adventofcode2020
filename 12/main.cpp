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
    int commands_followed = 0;
    std::string line;
    std::getline(input, line);
    while(input.good()){
        action movement = parse(line);
        switch(movement.first){
        case 'N':
            y += movement.second;
            //std::cout << "Moved north " << movement.second << " units" << std::endl;
            break;
        case 'E':
            x += movement.second;
            //std::cout << "Moved east  " << movement.second << " units" << std::endl;
            break;
        case 'S':
            y -= movement.second;
            //std::cout << "Moved south " << movement.second << " units" << std::endl;
            break;
        case 'W':
            x -= movement.second;
            //std::cout << "Moved west  " << movement.second << " units" << std::endl;
            break;
        case 'F':
            //std::cout << "Moved forward " << movement.second << " units facing ";
            switch(direction){
                case 0:
                    //std::cout << "east" << std::endl;
                    x += movement.second;
                    break;
                case 90:
                    //std::cout << "north" << std::endl;
                    y += movement.second;
                    break;
                case 180:
                    //std::cout << "west" << std::endl;
                    x -= movement.second;
                    break;
                case 270:
                    //std::cout << "south" << std::endl;
                    y -= movement.second;
                    break;
            }
            break;
        case 'L':
            direction += movement.second;
            direction %= 360;
            //std::cout << "Turned left " << movement.second << " degrees" << std::endl;
            break;
        case 'R':
            direction -= movement.second;
            if(direction < 0)
                direction += 360;
            //std::cout << "Turned right " << movement.second << " degrees" << std::endl;
            break;
        }
        //std::cout << "Ship is at " << x << ", " << y << " facing " << direction << " degrees" << std::endl;
        commands_followed++;
        std::getline(input, line);
    }

    std::cout << "The boat is at " << x << ", " << y << ": " << abs(x) + abs(y) << " units from the start" << std::endl;
    std::cout << commands_followed << " commands issued" << std::endl;
    return 0;
}