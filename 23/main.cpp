#include <iostream>
#include <fstream>
#include <list>
#include <unordered_map>

void play_move(std::list<int> &cups, int minimum, int maximum){
    int front = cups.front();
    cups.pop_front();
    int target = front - 1;
    std::list<int> temp;
    for(int i = 0; i < 3; i++){
        temp.push_back(cups.front());
        cups.pop_front();
    }
    std::unordered_map<int, int> index_of;
    int i = 0;
    for(auto it = cups.begin(); it != cups.end(); it++, i++){
        index_of.insert(std::make_pair(*it, i));
    }
    std::unordered_map<int, int>::iterator it;
    while((it = index_of.find(target)) == index_of.end()){
        target--;
        if(target < minimum){
            target = maximum;
        }
    }
    auto cups_pos = cups.begin();
    //Position cups_pos to point to the element directly clockwise of target
    //  Since list.insert() inserts items _before_ the specified element, to insert them
    //  _after_ the target we must insert them before the item after the target
    for(int i = 0; i <= it->second; i++, cups_pos++);
    cups.insert(cups_pos, temp.begin(), temp.end());
    cups.push_back(front);
}

void print_cups(std::list<int> cups){
    for(auto it = cups.begin(); it != cups.end(); it++){
        std::cout << *it << " ";
    }
    std::cout << std::endl;
}

void print_output(std::list<int> cups){
    while(cups.front() != 1){
        cups.push_back(cups.front());
        cups.pop_front();
    }
    cups.pop_front();
    while(cups.size() > 0){
        std::cout << cups.front();
        cups.pop_front();
    }
    std::cout << std::endl;
}

int main(int argc, char** argv){
    std::string filename = "input.txt";
    std::ifstream input;
    int moves = 100;
    if(argc >= 2){
        filename = argv[1];
    }
    if(argc >= 3){
        moves = atoi(argv[2]);
    }
    input.open(filename, std::ifstream::in);

    std::list<int> cups;
    int minimum = 100, maximum = 0;
    std::string line;
    while(input.good()){
        std::getline(input, line);
        for(int i = 0; i < line.size(); i++){
            int curr = line[i] - '0';
            if(curr > maximum){
                maximum = curr;
            }
            if(curr < minimum){
                minimum = curr;
            }
            cups.push_back(curr);
        }
    }
    print_cups(cups);
    for(int i = 0; i < moves; i++){
        play_move(cups, minimum, maximum);
    }
    print_cups(cups);
    print_output(cups);

    return 0;
}