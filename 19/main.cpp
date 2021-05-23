#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>

class rule {
public:
    rule(std::unordered_map<int, std::string>& rules, int id){
        this->id = id;
        size_t index = rules[id].find("\"");
        if(index != std::string::npos){
            match = rules[id].substr(index + 1, 1);
            return;
        }
        index = 0;
        while(index != std::string::npos){
            if(index != 0)
                index += 3;
            match_groups.push_back(parse_int_list(rules[id].substr(index, rules[id].find(" | ", index) - index)));
            index = rules[id].find(" | ", index);
        }
    }

    void print(){
        for(int i = 0; i < match_groups.size(); i++){
            for(int j = 0; j < match_groups[i].size(); j++){
                std::cout << match_groups[i][j] << " ";
            }
            if(i + 1 < match_groups.size())
                std::cout << "| ";
        }
        std::cout << std::endl;
    }

    void print(std::unordered_map<int, rule*>& parsed_rules){
        if(match.size() != 0){
            std::cout << match;
            return;
        }
        if(id == 8 || id == 11){
            std::cout << "(";
            parsed_rules[42]->print(parsed_rules);
            std::cout << ")+";
            if(id == 8)
                return;
        }
        if(id == 11){
            std::cout << "(";
            parsed_rules[31]->print(parsed_rules);
            std::cout << ")+";
            return;
        }
        std::cout << "(";
        for(int i = 0; i < match_groups.size(); i++){
            for(int j = 0; j < match_groups[i].size(); j++){
                parsed_rules[match_groups[i][j]]->print(parsed_rules);
            }
            if(i + 1 < match_groups.size())
                std::cout << "|";
        }
        std::cout << ")";
    }

    bool check(std::unordered_map<int, rule*>& parsed_rules, std::string to_match, bool debug){
        return check_helper(parsed_rules, 0, to_match, 0, debug) == to_match.size();
    }
    
    std::vector<std::string> can_match(std::unordered_map<int, rule*>& parsed_rules, int loops = 0){
        std::vector<std::string> to_return;
        if(match.size() != 0){
            to_return.push_back(match);
            return to_return;
        }
        for(int i = 0; i < match_groups.size(); i++){
            std::vector<std::vector<std::string>> to_concatenate;
            for(int j = 0; j < match_groups[i].size(); j++){
                if(match_groups[i][j] == id && loops <= 0){
                    continue;
                } else if(match_groups[i][j] == id && loops > 0){
                    std::cout << "Rule " << id << " looping!" << std::endl;
                    to_concatenate.push_back(parsed_rules[match_groups[i][j]]->can_match(parsed_rules, loops - 1));
                } else {
                    to_concatenate.push_back(parsed_rules[match_groups[i][j]]->can_match(parsed_rules, loops));
                }
            }
            if(to_concatenate.size() == 1){
                for(int b = 0; b < to_concatenate[0].size(); b++){
                    to_return.push_back(to_concatenate[0][b]);
                }
            } else {
                for(int j = 0; j < to_concatenate.size() - 1; j++){
                    for(int k = j + 1; k < to_concatenate.size(); k++){
                        for(int a = 0; a < to_concatenate[j].size(); a++){
                            for(int b = 0; b < to_concatenate[k].size(); b++){
                                to_return.push_back(to_concatenate[j][a] + to_concatenate[k][b]);
                            }
                        }
                    }
                }
            }
        }
        return to_return;
    }

private:
    std::vector<std::vector<int>> match_groups;
    std::string match;
    int id;

    std::vector<int> parse_int_list(std::string list){
        size_t index = 0;
        std::vector<int> sub_rules;
        while(index != std::string::npos){
            if(index != 0){
                index++;
            }
            sub_rules.push_back(std::stoi(list.substr(index, list.find(" ", index) - index)));
            index = list.find(" ", index);
        }
        return sub_rules;
    }

    int check_helper(std::unordered_map<int, rule*>& parsed_rules, int index, std::string to_match, int level, bool debug){
        if(debug){
            for(int i = 0; i < level; i++){
                std::cout << "    ";
            }
            std::cout << "Rule " << id << " attempting to match " << to_match << " at index " << index << std::endl;
        }
        if(match.size() > 0 && to_match.substr(index, 1) == match){
            if(debug){
                for(int i = 0; i <= level; i++){
                    std::cout << "    ";
                }
                std::cout << "Matched using own match of " << match << std::endl;
            }
            return 1;
        }
        int matched = 0, greatest_match = 0;
        for(int i = 0; i < match_groups.size() && index + matched != to_match.size(); i++){
            matched = 0;
            for(int j = 0; j < match_groups[i].size(); j++){
                int value = parsed_rules[match_groups[i][j]]->check_helper(parsed_rules, matched + index, to_match, level + 1, debug);
                if(value == 0){
                    if(debug){
                        for(int i = 0; i <= level + 1; i++){
                            std::cout << "    ";
                        }
                        std::cout << "failed" << std::endl;
                    }
                    matched = 0;
                    break;
                }
                matched += value;
            }
            if(matched != 0){
                return matched;
            }
        }
        return matched;
    }
};

int main(int argc, char** argv){
    std::string filename = "input.txt";
    std::ifstream input;
    int rule_to_check = 42, num_loops = 0;
    if(argc >= 2){
        filename = argv[1];
    } 
    if(argc >= 3){
        rule_to_check = atoi(argv[2]);
    }
    if(argc >= 4){
        num_loops = atoi(argv[3]);
    }
    input.open(filename, std::ifstream::in);

    std::unordered_map<int, std::string> text_rules;
    std::string line;
    std::getline(input, line);
    while(input.good() && line.size() > 0){
        int rule_index = std::stoi(line.substr(0, line.find(":")));
        text_rules.emplace(rule_index, line.substr(line.find(" ") + 1));
        std::getline(input, line);
    }
    std::unordered_map<int, rule*> parsed_rules;
    for(auto it = text_rules.begin(); it != text_rules.end(); it++){
        parsed_rules.emplace(it->first, new rule(text_rules, it->first));
    }
    int matches = 0;
    while(input.good()){
        std::getline(input, line);
        if(line.size() == 0){
            continue;
        }
        //std::cout << line << std::endl;
        //matches += parsed_rules[0]->check(parsed_rules, line, false) ? 1 : 0;
    }    
    std::cout << "^";
    parsed_rules[0]->print(parsed_rules);
    std::cout << "$";
    
    return 0;
}