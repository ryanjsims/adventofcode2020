#include <iostream>
#include <fstream>
#include <list>
#include <unordered_map>

void build_index_map(std::list<int> *cups, std::unordered_map<int, std::list<int>::iterator> *output){
    for(auto it = cups->begin(); it != cups->end(); it++){
        output->insert(std::make_pair(*it, it));
    }
}

void print_comparison(std::list<int> *cups, std::unordered_map<int, std::list<int>::iterator> *index_of);

void print_cups(std::list<int> *cups, std::list<int>::iterator pos);

void play_move(std::list<int> *cups, int minimum, int maximum, std::list<int>::iterator pos, std::unordered_map<int, std::list<int>::iterator> *index_of){
    int front = *pos;
    pos++;
    int target = front - 1;
    std::list<int> temp;
    //print_comparison(cups, index_of);
    for(int i = 0; i < 3; i++){
        if(pos == cups->end())
            pos = cups->begin();
        temp.push_back(*pos);
        //Remove elements from index map
        index_of->erase(*pos);
        pos = cups->erase(pos);
    }
    std::unordered_map<int, std::list<int>::iterator>::iterator index_it;
    while((index_it = index_of->find(target)) == index_of->end()){
        target--;
        if(target < minimum){
            target = maximum;
        }
    }
    //Position index_it->second to point to the element directly clockwise of target
    //  Since list.splice() inserts items _before_ the specified element, to insert them
    //  _after_ the target we must insert them before the item after the target
    auto next = index_it->second;
    next++;
    //print_cups(cups, pos);
    cups->insert(next, temp.begin(), temp.end());
    //print_cups(cups, pos);

    //Replace cups that were removed from the index map into the index map
    auto curr = index_it->second;
    curr++;
    for(; curr != next; curr++){
        index_of->insert(std::make_pair(*curr, curr));
    }
}

void print_comparison(std::list<int> *cups, std::unordered_map<int, std::list<int>::iterator> *index_of){
    for(auto it = cups->begin(); it != cups->end(); it++){
        std::unordered_map<int, std::list<int>::iterator>::iterator it2;
        if((it2 = index_of->find(*it)) == index_of->end())
            std::cout << *it << ", null" << std::endl;
        else
            std::cout << *it << ", " << *(it2->second) << std::endl;
    }
}

void print_cups(std::list<int> *cups, std::list<int>::iterator pos){
    for(auto it = cups->begin(); it != cups->end(); it++){
        if(it == pos)
            std::cout << "(";
        else
            std::cout << " ";
        std::cout << *it;
        if(it == pos)
            std::cout << ")";
        else
            std::cout << " ";

        std::cout << " ";
    }
    std::cout << std::endl;
}

void print_output(
    #ifdef PART1
    std::list<int> *cups
    #else
    std::unordered_map<int, std::list<int>::iterator> *index_of
    #endif
){
#ifdef PART1
    while(cups->front() != 1){
        cups->push_back(cups->front());
        cups->pop_front();
    }
    cups->pop_front();
    while(cups->size() > 0){
        std::cout << cups->front();
        cups->pop_front();
    }
    std::cout << std::endl;
#else
    auto one_pos = index_of->find(1)->second;
    one_pos++;
    uint64_t val_left = *one_pos;
    
    one_pos++;
    uint64_t val_right = *one_pos;
    uint64_t output = val_left * val_right;
    std::cout << val_left << " * " << val_right << " = " << output << std::endl;
#endif
}

int main(int argc, char** argv){
    std::string filename = "input.txt";
    std::ifstream input;
    int moves = 100;
    #ifdef PART2
    moves = 10000000;
    #endif
    if(argc >= 2){
        filename = argv[1];
    }
    if(argc >= 3){
        moves = atoi(argv[2]);
    }
    input.open(filename, std::ifstream::in);
    std::unordered_map<int, std::list<int>::iterator> index_map;
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
#ifdef PART2
        for(int i = maximum + 1; i <= 1000000; i++){
            cups.push_back(i);
        }
        maximum = 1000000;
#endif
    }
    build_index_map(&cups, &index_map);
    auto curr_pos = cups.begin();
    for(int i = 0; i < moves; i++){
        #ifdef PART1
        print_cups(&cups, curr_pos);
        #endif
        play_move(&cups, minimum, maximum, curr_pos, &index_map);
        curr_pos++;
        if(curr_pos == cups.end()){
            curr_pos = cups.begin();
        }
    }
#ifdef PART1
    print_cups(&cups, curr_pos);
    print_output(&cups);
#else
    print_output(&index_map);
#endif

    return 0;
}