#include <iostream>
#include <fstream>
#include <vector>
#include <array>
#include <unordered_map>

enum class State {
    rules,
    myticket,
    othertickets
};

typedef std::array<int, 20> ticket;
typedef std::pair<std::pair<int, int>, std::pair<int, int>> disjoint_range;

ticket parse_ticket(std::string line){
    ticket to_return = {0};
    int index = 0;
    for(int i = 0; i < 20; i++){
        to_return[i] = std::stoi(line.substr(index, line.find(",", index) - index));
        index = line.find(",", index) + 1;
    }
    return to_return;
}

std::string rule_name(std::string line){
    return line.substr(0, line.find(":"));
}

disjoint_range parse_rule(std::string line){
    std::pair<int, int> first, second;
    int index = line.find_first_of("1234567890");
    first.first = std::stoi(line.substr(index, line.find("-", index) - index));
    index = line.find("-", index) + 1;
    first.second = std::stoi(line.substr(index, line.find(" ", index) - index));
    index = line.find_first_of("1234567890", line.find(" ", index));
    second.first = std::stoi(line.substr(index, line.find("-", index) - index));
    index = line.find("-", index) + 1;
    second.second = std::stoi(line.substr(index, line.find(" ", index) - index));
    return std::make_pair(first, second);
}

void print_range(disjoint_range range){
    std::cout << range.first.first << "-" << range.first.second << " or " << range.second.first << "-" << range.second.second;
}

bool validate_value(disjoint_range range, int value){
    return value >= range.first.first && value <= range.first.second || value >= range.second.first && value <= range.second.second;
}

int validate_ticket(std::unordered_map<std::string, disjoint_range> rules, ticket t){
    for(auto tickval = t.begin(); tickval != t.end(); tickval++){
        bool passed = false;
        for(auto ruleit = rules.begin(); ruleit != rules.end(); ruleit++){
            if(validate_value(ruleit->second, *tickval)){
                passed = true;
            }
        }
        if(!passed){
            return *tickval;
        }
    }
    return 0;
}

int main(){
    State state = State::rules;
    std::ifstream input;
    input.open("input.txt", std::ifstream::in);

    std::string line;
    std::unordered_map<std::string, disjoint_range> rules;
    ticket mine;
    std::vector<ticket> others;
    std::getline(input, line);
    while(input.good()){
        if(line == ""){
            std::getline(input, line);
            continue;
        }
        switch(state){
        case State::rules:
            if(line == "your ticket:"){
                state = State::myticket;
                break;
            }
            rules.emplace(rule_name(line), parse_rule(line));
            break;
        case State::myticket:
            if(line == "nearby tickets:"){
                state = State::othertickets;
                break;
            }
            mine = parse_ticket(line);
            break;
        case State::othertickets:
            others.push_back(parse_ticket(line));
            break;
        }
        std::getline(input, line);
    }
    /*std::cout << "Rules:" << std::endl;
    for(auto it = rules.begin(); it != rules.end(); it++){
        std::cout << it->first << ": ";
        print_range(it->second);
        std::cout << std::endl;
    }*/
    std::cout << "There are " << others.size() << " nearby tickets" << std::endl;
    int error_rate = 0, num_errors = 0;
    int i = 0;
    while(i < others.size()){
        int error = validate_ticket(rules, others[i]);
        error_rate += error;
        if(error != 0){
            others.erase(others.begin() + i);
            num_errors++;
        } else {
            i++;
        }
    }
    std::cout << num_errors << " erroneous tickets were detected, with an error rate of " << error_rate << std::endl;
    std::cout << others.size() << " other tickets remain" << std::endl;
    return 0;
}