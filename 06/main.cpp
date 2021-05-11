#include <iostream>
#include <fstream>
#include <set>
#include <vector>
#include <algorithm>

int get_answers(std::vector<std::set<char>> answers){
    std::set<char> temp1, temp2;
    int total = 0;
    if(answers.size() == 1){
        total = answers[0].size();
    } else {
        std::set<char>* output = &temp1, *input = &temp1;
        std::set_intersection(answers[0].begin(), answers[0].end(), 
            answers[1].begin(), answers[1].end(), 
            std::inserter(*output, output->begin()));
        for(int i = 2; i < answers.size(); i++){
            output = output == &temp1 ? &temp2 : &temp1;
            output->clear();
            std::set_intersection(input->begin(), input->end(), answers[i].begin(), answers[i].end(), 
                                  std::inserter(*output, output->begin()));
            input = output;
        }
        total = output->size();
    }
    return total;
}

int main(){
    std::ifstream input;
    input.open("input.txt", std::ifstream::in);

    int total = 0, group = 0;
    std::vector<std::set<char>> answers;
    std::string line;
    std::getline(input, line);
    while(input.good()){
        if(line.size() == 0){
            total += get_answers(answers);
            answers.clear();
            group++;
        } else {
            answers.push_back(std::set<char>());
            for(int i = 0; i < line.size(); i++){
                answers[answers.size() - 1].insert(line[i]);
            }
        }
        std::getline(input, line);
    }
    if(answers.size() > 0){
        total += get_answers(answers);
    }
    std::cout << total << " answers logged" << std::endl;
    return 0;
}