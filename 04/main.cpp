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
        return info.size() == 8 || (info.size() == 7 && info.count("cid") == 0);
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
                p.print();
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
        p.print();
    }
    std::cout << "There were " << valid << " valid passports" << std::endl;
    return 0;
}