#include <iostream>
#include <fstream>

bool validate(std::string policy, std::string password);

int main(int argc, char** argv){
    std::ifstream input;
    input.open("input.txt", std::ifstream::in);

    std::string line;
    int valid = 0;
    while(input.good()){
        std::getline(input, line);
        if(line.size() == 0){
            break;
        }
        std::string policy = line.substr(0, line.find(':'));
        std::string password = line.substr(line.find(':') + 2);
        if(validate(policy, password)){
            valid++;
        }
    }
    std::cout << valid << " valid passwords" << std::endl;
    return 0;
}

bool validate(std::string policy, std::string password){
    std::string range = policy.substr(0, policy.find(' '));
    char required = policy[policy.size() - 1];
    int min = atoi(range.substr(0, range.find('-')).c_str());
    int max = atoi(range.substr(range.find('-') + 1).c_str());
    /*int count = 0;
    int index = 0;
    while((index = password.find(required, index)) != std::string::npos){
        count++;
        index++;
    }*/
    return (password[min - 1] == required) != (password[max - 1] == required);
}