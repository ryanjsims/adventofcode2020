#include <iostream>
#include <fstream>
#include <unordered_map>
#include <bitset>
#include <iomanip>

void update_masks(std::string line, std::bitset<36>& or_mask, std::bitset<36>& and_mask){
    if(line.substr(0, 4) != "mask")
        return;
    std::string mask = line.substr(line.find_first_of("01X"));
    if(mask.size() != 36){
        std::cerr << "Incorrect mask size: " << mask.size() << std::endl;
        exit(1);
    }
    for(int i = 0; i < 36; i++){
        switch(mask[35 - i]){
            case 'X':
                and_mask |= 1ul << i;
                or_mask &= ~(1ul << i);
                break;
            case '0':
                and_mask &= ~(1ul << i);
                or_mask &= ~(1ul << i);
                break;
            case '1':
                and_mask |= 1ul << i;
                or_mask |= 1ul << i;
                break;
        }
    }
}

void update_memory(std::string line, std::unordered_map<int, uint64_t>& memory, 
                    std::bitset<36> or_mask, std::bitset<36> and_mask){
    int start = line.find("[") + 1;
    int end = line.find("]");
    int address = std::stoi(line.substr(start, end - start));
    uint64_t data = std::stoul(line.substr(line.find_first_not_of(" =", end + 1)));
    data = data | or_mask.to_ulong();
    data = data & and_mask.to_ulong();
    if(memory.find(address) == memory.end()){
        memory.emplace(address, 0);
    }
    memory[address] = data;
    std::cout << "Set mem[" << address << "] = " << std::setw(8) << std::right << data << " (0b" << std::bitset<36>(data) << ")" << std::endl;
}

int main(){
    std::ifstream input;
    input.open("input.txt", std::ifstream::in);

    std::string line;
    std::string mask;
    std::bitset<36> or_mask = 0, and_mask = 0xFFFFFFFFF;
    std::unordered_map<int, uint64_t> memory;
    try{
        while(input.good()){
            std::getline(input, line);
            if(line.substr(0, 4) == "mask"){
                update_masks(line, or_mask, and_mask);
                std::cout << "Updated masks to: " << std::endl;
                std::cout << "\t          or:  0b" << or_mask << std::endl;
                std::cout << "\t          and: 0b" << and_mask << std::endl;
            } else if(line.substr(0, 3) == "mem") {
                update_memory(line, memory, or_mask, and_mask);
            }
        }
    } catch(std::invalid_argument e){
        std::cout << line << std::endl;
        exit(1);
    }

    uint64_t sum = 0;
    for(auto it = memory.begin(); it != memory.end(); it++){
        sum += it->second;
    }
    std::cout << "Values in memory sum to " << sum << std::endl;

    return 0;
}