#include <iostream>
#include <fstream>
#include <unordered_map>
#include <bitset>
#include <iomanip>
#include <vector>

#define version 2

std::vector<uint64_t> generate_addresses(uint64_t address, std::bitset<36> or_mask, std::bitset<36> and_mask, std::vector<int> xloc);

void update_masks(std::string line, std::bitset<36>& or_mask, std::bitset<36>& and_mask, std::vector<int>& xloc){
    if(line.substr(0, 4) != "mask")
        return;
    xloc.clear();
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
                xloc.push_back(i);
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

void update_memory(std::string line, std::unordered_map<uint64_t, uint64_t>& memory, 
                    std::bitset<36> or_mask, std::bitset<36> and_mask, std::vector<int> xloc){
    int start = line.find("[") + 1;
    int end = line.find("]");
    uint64_t address = std::stoul(line.substr(start, end - start));
    uint64_t data = std::stoul(line.substr(line.find_first_not_of(" =", end + 1)));
    if(version == 1){
        data = data | or_mask.to_ulong();
        data = data & and_mask.to_ulong();
        if(memory.find(address) == memory.end()){
            memory.emplace(address, 0);
        }
        memory[address] = data;
        //std::cout << "Set mem[" << address << "] = " << std::setw(8) << std::right << data << " (0b" << std::bitset<36>(data) << ")" << std::endl;
    } else if(version == 2){
        std::vector<uint64_t> addresses = generate_addresses(address, or_mask, and_mask, xloc);
        for(int i = 0; i < addresses.size(); i++){
            if(memory.find(addresses[i]) == memory.end()){
                memory.emplace(addresses[i], 0);
            }
            memory[addresses[i]] = data;
            #ifdef debug
            std::cout << "Set mem[" << std::bitset<8>(addresses[i]) << "] = " << std::setw(8) << std::right << data << " (0b" << std::bitset<36>(data) << ")" << std::endl;
            #endif
        }
    }
}

std::vector<uint64_t> generate_addresses(uint64_t address, std::bitset<36> or_mask, std::bitset<36> and_mask, std::vector<int> xloc){
    std::vector<uint64_t> addresses;
    address |= or_mask.to_ulong();
    //address &= and_mask.to_ulong();
    for(uint64_t i = 0; i < 1 << xloc.size(); i++){
        for(int j = 0; j < xloc.size(); j++){
            if(i & 1 << j){
                address |= 1ul << xloc[j];
            } else {
                address &= ~(1ul << xloc[j]);
            }
        }
        addresses.push_back(address);
    }
    return addresses;
}

int main(){
    std::ifstream input;
    input.open("input.txt", std::ifstream::in);

    std::string line;
    std::string mask;
    std::bitset<36> or_mask = 0, and_mask = 0xFFFFFFFFF;
    std::unordered_map<uint64_t, uint64_t> memory;
    std::vector<int> xloc;
    try{
        while(input.good()){
            std::getline(input, line);
            if(line.substr(0, 4) == "mask"){
                update_masks(line, or_mask, and_mask, xloc);
                #ifdef debug
                std::cout << "Updated masks to: " << std::endl;
                std::cout << "\t          or:  0b" << or_mask << std::endl;
                std::cout << "\t          and: 0b" << and_mask << std::endl;
                #endif
            } else if(line.substr(0, 3) == "mem") {
                update_memory(line, memory, or_mask, and_mask, xloc);
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