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
    return 0;
}