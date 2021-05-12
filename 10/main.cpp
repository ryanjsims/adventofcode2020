#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

int main(){
    std::ifstream input;
    input.open("input.txt", std::ifstream::in);
    int differences[3] = {1, 0, 1};
    std::vector<int> adapters;
    std::string line;
    std::getline(input, line);
    while(input.good()){
        adapters.push_back(std::stoi(line));
        std::getline(input, line);
    }

    std::sort(adapters.begin(), adapters.end());
    for(int i = 0; i < adapters.size() - 1; i++){
        differences[adapters[i+1] - adapters[i] - 1]++;
    }
    std::cout << "There are " << differences[0] << " 1 jolt gaps and " << differences[2] << " 3 jolt gaps" << std::endl;
    std::cout << "Their product is " << differences[0] * differences[2] << std::endl;
    return 0;
}