#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>

class Passport {
private:
    std::unordered_map<std::string, std::string> info;
public:
    Passport(std::vector<std::string> tokens){
        for(int i = 0; i < tokens.size(); i++){
            int splitloc = tokens[i].find(':');
            std::string key, value;
            key = tokens[i].substr(0, splitloc);
            value = tokens[i].substr(splitloc + 1);
            info.emplace(key, value);
        }
    }

    bool valid(){
        bool valid = info.size() == 8 || (info.size() == 7 && info.count("cid") == 0);
        if(valid){
            valid &= validate_year("byr", 1920, 2002);
            valid &= validate_year("iyr", 2010, 2020);
            valid &= validate_year("eyr", 2020, 2030);
            valid &= validate_height();
            valid &= validate_eyes();
            valid &= validate_hair();
            valid &= validate_pid();
        }
        return valid;
    }

    bool validate_year(std::string key, int minimum, int maximum){
        int year = std::stoi(info[key]);
        return info[key].size() == 4 && year >= minimum && year <= maximum;
    }

    bool validate_height(){
        if(info["hgt"].size() < 4)
            return false;
        std::string units = info["hgt"].substr(info["hgt"].size() - 2);
        if(units != "cm" && units != "in"){
            return false;
        }
        int value = std::stoi(info["hgt"].substr(0, info["hgt"].size() - 2));
        if(units == "cm"){
            return value >= 150 && value <= 193;
        }
        else if(units == "in"){
            return value >= 59 && value <= 76;
        }
        return false;
    }

    bool validate_hair(){
        if(info["hcl"][0] != '#' || info["hcl"].size() != 7)
            return false;
        for(int i = 1; i < info["hcl"].size(); i++){
            if(!(info["hcl"][i] >= 'a' && info["hcl"][i] <= 'f' || info["hcl"][i] >= '0' && info["hcl"][i] <= '9'))
                return false;
        }
        return true;
    }

    bool validate_eyes(){
        std::string valid_colors[7] = {"amb", "blu", "brn", "gry", "grn", "hzl", "oth"};
        for(int i = 0; i < 7; i++){
            if(info["ecl"] == valid_colors[i]){
                return true;
            }
        }
        return false;
    }

    bool validate_pid(){
        return info["pid"].size() == 9;
    }

    void print(){
        std::cout << "Passport" << std::endl;
        for(auto it = info.begin(); it != info.end(); it++){
            std::cout << '\t' << it->first << ": " << it->second << std::endl;
        }
        std::cout << std::endl;
    }
};

int main(int argc, char** argv){
    std::ifstream input;
    input.open("input.txt", std::ifstream::in);

    int valid = 0;

    std::string line;
    std::vector<std::string> tokens;
    std::getline(input, line);
    while(input.good()){
        if(line.size() == 0){
            Passport p(tokens);
            if(p.valid()){
                valid++;
            }
            tokens.clear();
            std::getline(input, line);
        }
        size_t currpos = 0;
        size_t spacepos = line.find(' ');
        while(spacepos != std::string::npos){
            tokens.push_back(line.substr(currpos, spacepos - currpos));
            currpos = spacepos + 1;
            spacepos = line.find(' ', currpos);
        }
        tokens.push_back(line.substr(currpos));
        std::getline(input, line);
    }
    Passport p(tokens);
    if(p.valid()){
        valid++;
    }
    std::cout << "There were " << valid << " valid passports" << std::endl;
    return 0;
}