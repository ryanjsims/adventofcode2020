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

    bool check(std::unordered_map<int, rule*>& parsed_rules, std::string to_match, bool debug){
        return check_helper(parsed_rules, 0, to_match, 0, debug) == to_match.size();
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
        int matched = 0;
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
    if(argc == 2){
        filename = argv[1];
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
        matches += parsed_rules[0]->check(parsed_rules, line, false) ? 1 : 0;
    }    
    std::cout << matches << std::endl;
    
    return 0;
}