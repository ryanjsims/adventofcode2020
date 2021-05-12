#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <math.h>

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
    std::unordered_map<int, int> ones_length_to_count;
    int current_streak_length = 1;
    for(int i = 0; i < adapters.size() - 1; i++){
        if(adapters[i+1] - adapters[i] == 1){
            current_streak_length++;
        } else if(current_streak_length != 0) {
            if(ones_length_to_count.find(current_streak_length) == ones_length_to_count.end()){
                ones_length_to_count.emplace(current_streak_length, 0);
            }
            ones_length_to_count[current_streak_length]++;
            current_streak_length = 0;
        }
        differences[adapters[i+1] - adapters[i] - 1]++;
    }
    if(current_streak_length != 0){
         if(ones_length_to_count.find(current_streak_length) == ones_length_to_count.end()){
            ones_length_to_count.emplace(current_streak_length, 0);
        }
        ones_length_to_count[current_streak_length]++;
        current_streak_length = 0;
    }
    std::cout << "There are " << differences[0] << " 1 jolt gaps and " << differences[2] << " 3 jolt gaps" << std::endl;
    std::cout << "Their product is " << differences[0] * differences[2] << std::endl;

    uint64_t total_combos = 1;
    for(auto it = ones_length_to_count.begin(); it != ones_length_to_count.end(); it++){
        std::cout << it->first << ": " << it->second << std::endl;
        switch(it->first){
        case 2:
            total_combos *= pow(2, it->second);
            break;
        case 3:
            total_combos *= pow(4, it->second);
            break;
        case 4:
            total_combos *= pow(7, it->second);
            break;
        }
    }
    std::cout << "Combos: " << total_combos << std::endl;
    return 0;
}