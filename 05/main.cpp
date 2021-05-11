#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

int main(int argc, char** argv){
    std::ifstream input;
    input.open("input.txt", std::ifstream::in);

    std::string line;
    std::getline(input, line);
    std::vector<int> seat_ids;
    while(input.good()){
        int seat_id = 0;
        for(int i = 0; i < line.size(); i++){
            seat_id <<= 1;
            if(line[i] == 'B' || line[i] == 'R'){
                seat_id += 1;
            }
        }
        seat_ids.push_back(seat_id);
        std::getline(input, line);
    }
    std::sort(seat_ids.begin(), seat_ids.end());
    for(int i = 0; i < seat_ids.size() - 1; i++){
        if(seat_ids[i] != seat_ids[i + 1] - 1){
            std::cout << seat_ids[i] + 1 << std::endl;
        }
    }
    return 0;
}