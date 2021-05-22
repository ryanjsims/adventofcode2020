#include <iostream>
#include <fstream>

int main(int argc, char** argv){
    std::string filename = "input.txt";
    std::ifstream input;
    if(argc == 2){
        filename = argv[1];
    }
    input.open(filename, std::ifstream::in);

    std::string line;
    std::getline(input, line);
    while(input.good()){
        
        std::getline(input, line);
    }

    return 0;
}