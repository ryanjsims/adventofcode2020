#include <iostream>
#include <fstream>
#include <unordered_map>
#include <thread>
#include <chrono>

using namespace std::chrono_literals;

int main(){
    std::ifstream input;
    input.open("input.txt", std::ifstream::in);

    std::unordered_map<int, int> numbers;
    std::string line;
    std::getline(input, line);
    std::size_t index = 0;
    int turn = 1;
    int prev = 0;
    bool first;
    while(index != std::string::npos){
        int value = std::stoi(line.substr(index, line.find(",", index) - index));
        index = line.find(",", index);
        first = numbers.find(value) == numbers.end();
        if(first){
            std::cout << "First time " << value << " has been seen" << std::endl;
            numbers.emplace(value, turn);
        }
        numbers[value] = turn;
        prev = value;
        std::cout << "Turn " << turn << ": " << value << std::endl;
        turn++;
        if(index == std::string::npos){
            break;
        }
        index++;
    }
    while(turn != 2021){
        int current = 0;
        char val;
        if(!first){
            current = (turn - 1) - numbers[prev];
            std::cout << prev <<  " was seen " << current << " turns ago" << std::endl;
        } else {
            std::cout << "First time " << prev << " has been seen" << std::endl;
        }
        first = numbers.find(current) == numbers.end();
        if(first){
            numbers.emplace(current, turn);
        }
        numbers[prev] = turn - 1;
        prev = current;
        std::cout << "Turn " << turn << ": " << current << std::endl;
        turn++;
    }


    return 0;
}