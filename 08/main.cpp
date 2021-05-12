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

    bool step(bool stop_on_loop=true, bool speculate=true){
        if(times_run.find(address) == times_run.end()){
            times_run.emplace(address, 0);
        } else if(stop_on_loop) {
            std::cout << address << "\t" << accumulator << "\t" << code[address] << std::endl;
            std::cout << "Accumulator was " << accumulator << " before looping" << std::endl;
            return false;
        }
        if(address == code.size()){
            std::cout << "Accumulator was " << accumulator << " before halting" << std::endl;
            exit(0);
        }

        times_run[address] += 1;
        std::pair<std::string, int> instruction = this->parse(code[address]);

        if(instruction.first == "acc"){
            accumulator += instruction.second;
            address++;
        } else if(instruction.first == "jmp"){
            if(speculate){
                this->speculate(instruction);
            }
            address += instruction.second;
        } else if(instruction.first == "nop"){
            if(speculate){
                this->speculate(instruction);
            }
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

    void run(bool speculate=false){
        while(step(true, speculate));
    }

    void speculate(std::pair<std::string, int> instruction){
        std::cout << "SPECULATING AT ADDRESS " << address << "..." << std::endl; //address << "\t" << accumulator << "\t" << code[address] << "  \tWould make address " << address + 1 << " as nop" << std::endl;
        int addr = address, acc = accumulator;
        std::unordered_map<int, int> old_times = times_run;
        if(instruction.first == "nop")
            address += instruction.second;
        else
            address++;
        while(step(true, false));
        std::cout << "FINISHED SPECULATING" << std::endl;
        address = addr;
        accumulator = acc;
        times_run = old_times;
    }
};

int main(){
    std::ifstream input;
    input.open("input.txt", std::ifstream::in);

    HandheldEmu emu(std::move(input));
    emu.run();
    return 0;
}