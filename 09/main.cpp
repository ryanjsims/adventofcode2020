#include <iostream>
#include <fstream>
#include <vector>
#define PREAMBLE 25

bool check_element(std::vector<uint64_t> data, int index){
    uint64_t value = data[index];
    for(int i = index - PREAMBLE; i < index - 1; i++){
        for(int j = i; j < index; j++){
            if(data[i] + data[j] == value){
                return true;
            }
        }
    }
    return false;
}

std::pair<int, int> find_range(std::vector<uint64_t> data, uint64_t target){
    int start = 0;
    while(start < data.size() && data[start] + data[start + 1] <= target){
        int index = start + 2;
        uint64_t sum = data[start] + data[start + 1];
        while(sum < target){
            sum += data[index];
            index++;
        }
        if(sum == target){
            return std::make_pair(start, index);
        }
        start++;
    }
    return std::make_pair(-1, -1);
}

int main(){
    std::ifstream input;
    input.open("input.txt", std::ifstream::in);

    std::vector<uint64_t> data;
    std::string line;
    std::getline(input, line);
    while(input.good()){
        data.push_back(std::stoul(line));
        std::getline(input, line);
    }

    int index = PREAMBLE;
    while(check_element(data, index)){
        index++;
    }
    std::cout << "Element at index " << index << ": " << data[index] << " is not the sum of two elements in the preamble" << std::endl;
    
    std::pair<int, int> range = find_range(data, data[index]);
    std::cout << "Elements in index range " << range.first << " to " << range.second - 1 << " sum to " << data[index] << std::endl;
    uint64_t min = data[index], max = 0;
    for(int i = range.first; i < range.second; i++){
        if(data[i] < min){
            min = data[i];
        }
        if(data[i] > max){
            max = data[i];
        }
    }
    std::cout << "The min and max of that range are " << min << " and " << max << std::endl;
    std::cout << "They sum to " << min + max << std::endl;
    return 0;
}