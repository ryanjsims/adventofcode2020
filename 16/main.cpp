#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <array>
#include <iomanip>
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
    for(int i = 0; i < t.size(); i++){
        bool passed = false;
        for(auto ruleit = rules.begin(); ruleit != rules.end(); ruleit++){
            if(validate_value(ruleit->second, t[i])){
                passed = true;
            }
        }
        if(!passed){
            return t[i];
        }
    }
    return -1;
}

int find_rule_index(std::string rule_name, disjoint_range range, std::vector<ticket> tickets, std::unordered_map<int, std::string> rule_locs, int padding, std::ostream& out){
    std::array<bool, 20> indexes;
    for(int i = 0; i < indexes.size(); i++){
        indexes[i] = true;
    }
    int index_count = 0;
    bool name_found = false;
    for(auto ittick = tickets.begin(); ittick != tickets.end(); ittick++){
        for(int i = 0; i < ittick->size(); i++){
            if(rule_locs.find(i) == rule_locs.end())
                indexes[i] &= validate_value(range, (*ittick)[i]);
            else if(rule_name != rule_locs[i])
                indexes[i] = false;
            else
                name_found = true;
        }
    }
    int valid_index = -1;
    for(int i = 0; i < indexes.size(); i++){
        if(indexes[i]){
            index_count++;
            valid_index = i;
        }
    }
    if(padding != 0){
        out << (index_count == 1 && !name_found ? "\033[44m" : "\033[40m") << rule_name << "\033[0m";
        for(int i = 0; i < padding; i++){
            out << " ";
        }
        std::string t = "\033[1;32m true\033[0m", f = "\033[1;31mfalse\033[0m";
        for(int i = 0; i < indexes.size(); i++){
            out << (indexes[i] ? t : f) << " ";
        }
        out << std::endl;
    }
    if(index_count == 1)
        return valid_index;
    return -1;
}

void draw_table(std::unordered_map<std::string, disjoint_range> rules, 
                std::unordered_map<int, std::string> rule_locs, std::vector<ticket> tickets){
    int max_size = 0;
    std::stringstream output("");
    for(auto it = rules.begin(); it != rules.end(); it++){
        if(it->first.size() > max_size)
            max_size = it->first.size();
    }
    for(int i = 0; i < max_size + 1; i++){
        output << " ";
    }
    for(int i = 0; i < 20; i++){
        output << std::setw(5) << i << " ";
    }
    output << std::endl;

    for(auto it = rules.begin(); it != rules.end(); it++){
        int index = find_rule_index(it->first, it->second, tickets, rule_locs, max_size - it->first.size() + 1, output);
    }
    output << "=====================================================================";
    output << "=====================================================================" << std::endl;
    std::cout << output.str();
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
    if(line != ""){
        others.push_back(parse_ticket(line));
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
        error_rate += error == -1 ? 0 : error;
        if(error != -1){
            others.erase(others.begin() + i);
            num_errors++;
        } else {
            i++;
        }
    }
    std::cout << num_errors << " erroneous tickets were detected, with an error rate of " << error_rate << std::endl;
    std::cout << others.size() << " other tickets remain" << std::endl;
    std::unordered_map<int, std::string> rule_locs;
    /*rule_locs.emplace(0, "arrival location");
    rule_locs.emplace(1, "arrival station");
    rule_locs.emplace(2, "wagon");
    rule_locs.emplace(3, "seat");
    rule_locs.emplace(4, "arrival track");
    rule_locs.emplace(5, "departure location");
    rule_locs.emplace(8, "duration");
    rule_locs.emplace(9, "price");
    rule_locs.emplace(11, "departure station");
    rule_locs.emplace(12, "zone");
    rule_locs.emplace(13, "departure time");
    rule_locs.emplace(14, "departure date");
    rule_locs.emplace(16, "train");
    rule_locs.emplace(17, "departure platform");*/
    //rule_locs.emplace(18, "route");
    //rule_locs.emplace(19, "departure track");
    others.push_back(mine);
    int prev = rule_locs.size();
    while(rule_locs.size() != 20){
        for(auto it = rules.begin(); it != rules.end(); it++){
            int index = find_rule_index(it->first, it->second, others, rule_locs, 0, std::cout);
            if(index != -1 && rule_locs.find(index) == rule_locs.end()){
                draw_table(rules, rule_locs, others);
                rule_locs.emplace(index, it->first);
            }
        }
        if(prev == rule_locs.size() || rule_locs.size() == 20){
            //draw_table(rules, rule_locs, others);
            break;
        }
        prev = rule_locs.size();
    }
    uint64_t result = 1;
    for(int i = 0; i < rule_locs.size(); i++){
        if(rule_locs[i].substr(0, 9) == "departure"){
            result *= mine[i];
        }
    }
    std::cout << result << std::endl;
    return 0;
}

