#include <iostream>
#include <fstream>

int main(int argc, char** argv){
    std::ifstream input;
    input.open("input.txt", std::ifstream::in);

    std::string line;
    std::getline(input, line);
    int max = 0;
    while(input.good()){
        int seat_id = 0;
        for(int i = 0; i < line.size(); i++){
            seat_id <<= 1;
            if(line[i] == 'B' || line[i] == 'R'){
                seat_id += 1;
            }
        }
        if(seat_id > 1023)
            std::cout << line << " " << seat_id << std::endl;
        if(seat_id > max){
            max = seat_id;
        }
        std::getline(input, line);
    }
    std::cout << max << std::endl;
    return 0;
}