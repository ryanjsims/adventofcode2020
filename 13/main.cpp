#include <iostream>
#include <fstream>
#include <vector>

int main(){
    std::ifstream input;
    input.open("input.txt", std::ifstream::in);

    std::string line;
    std::getline(input, line);
    int ts = std::stoi(line);
    std::getline(input, line);
    int index = 0;
    int mintime = ts;
    int min_id = 0;
    while(line.find(',', index) != std::string::npos){
        std::string bus_str = line.substr(index, line.find(',', index) - index);
        if(bus_str != "x"){
            int bus_id = std::stoi(bus_str);
            int new_time = bus_id - ts % bus_id;
            if(new_time < mintime){
                mintime = new_time;
                min_id = bus_id;
            }
        }
        index = line.find(',', index) + 1;
    }
    std::cout << "Minimum wait time is " << mintime << " minutes with bus " << min_id << std::endl;
    std::cout << "Product is " << mintime * min_id << std::endl;
    return 0;
}