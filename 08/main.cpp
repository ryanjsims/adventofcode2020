#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>

class HandheldEmu {
    int accumulator;
    int address;
    std::vector<std::string> code;
    std::unordered_map<int, int> times_run;
public:
    HandheldEmu(std::ifstream code_source){
        std::string line;
        std::getline(code_source, line);
        while(code_source.good()){
            code.push_back(line);
            std::getline(code_source, line);
        }
        accumulator = 0;
        address = 0;
    }

    bool step(bool stop_on_loop=true){
        if(times_run.find(address) == times_run.end()){
            times_run.emplace(address, 0);
        } else if(stop_on_loop) {
            std::cout << address << "\t" << accumulator << "\t" << code[address] << std::endl;
            std::cout << "Accumulator was " << accumulator << " before looping" << std::endl;
            return false;
        }
        times_run[address] += 1;
        std::cout << address << "\t" << accumulator << "\t" << code[address] << std::endl;
        std::pair<std::string, int> instruction = this->parse(code[address]);
        if(instruction.first == "acc"){
            accumulator += instruction.second;
            address++;
        } else if(instruction.first == "jmp"){
            address += instruction.second;
        } else if(instruction.first == "nop"){
            address++;
        } else {
            std::cerr << "Unrecognized instruction!" << std::endl;
            exit(EXIT_FAILURE);
        }
        return true;
    }

    std::pair<std::string, int> parse(std::string instruction){
        std::string op = instruction.substr(0, instruction.find(' '));
        int amt = std::stoi(instruction.substr(instruction.find(' ') + 1));
        return std::make_pair(op, amt);
    }

    void run(){
        while(step());
    }
};

int main(){
    std::ifstream input;
    input.open("input.txt", std::ifstream::in);

    HandheldEmu emu(std::move(input));
    emu.run();
    return 0;
}