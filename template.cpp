#include <iostream>
#include <fstream>

int main(){
    std::ifstream input;
    input.open("input.txt", std::ifstream::in);

    std::string line;
    std::getline(input, line);
    while(input.good()){
        
        std::getline(input, line);
    }

    return 0;
}