#include <iostream>
#include <fstream>
#include <unordered_set>

int main(){
    std::ifstream input;
    input.open("input.txt", std::ifstream::in);

    int total = 0, group = 0;
    std::unordered_set<char> answers;
    std::string line;
    std::getline(input, line);
    while(input.good()){
        if(line.size() == 0){
            total += answers.size();
            answers.clear();
            group++;
        } else {
            for(int i = 0; i < line.size(); i++){
                answers.insert(line[i]);
            }
        }
        std::getline(input, line);
    }
    if(answers.size() > 0){
        total += answers.size();
    }
    std::cout << total << " answers logged" << std::endl;
    return 0;
}