#include <iostream>
#include <fstream>

int main(int argc, char** argv){
    std::ifstream input;
    input.open("input.txt", std::ifstream::in);
    std::string line;
    std::getline(input, line);

    int y = 0;
    int xvals[5] = {0, 0, 0, 0, 0};
    int xincr[5] = {1, 3, 5, 7, 1};
    int trees[5] = {0, 0, 0, 0, 0};
    while(input.good()){
        for(int i = 0; i < 4; i++){
            if(line[xvals[i]] == '#')
                trees[i]++;
            xvals[i] = (xvals[i] + xincr[i]) % line.size();
        }
        if(y % 2 == 0){
            if(line[xvals[4]] == '#')
                trees[4]++;
            xvals[4] = (xvals[4] + xincr[4]) % line.size();
        }
        y++;
        std::getline(input, line);
    }
    int result = 1;
    for(int i = 0; i < 5; i++){
        result *= trees[i];
    }
    std::cout << "Result: " << result << std::endl;
    return 0;
}