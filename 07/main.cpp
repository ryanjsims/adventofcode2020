#include <iostream>
#include <fstream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

class Bag;

struct Edge {
    int amount;
    Bag *container, *containee;
};

class Bag {
    static int id;
public:
    std::string kind;
    std::vector<Edge> contained_by, contains;
    int bid;
    Bag(std::string kind){
        this->kind = kind;
        bid = id;
        id++;
    }
};

int Bag::id = 0;

//mirrored olive bags contain 4 faded blue bags, 2 posh purple bags, 1 striped brown bag.
void parse_rule(std::string line, std::vector<Bag*>& bags, std::unordered_map<std::string, int>& names_to_ids){
    int index = line.find(" contain "), curr = 0;
    std::string container = line.substr(0, index);
    std::vector<std::pair<std::string, int>> containees;
    index += 7;
    while(index != std::string::npos){
        curr = index + 2;
        index = line.find_first_of(",.", curr);
        if(index == std::string::npos)
            break;
        std::string containee = line.substr(curr, index - curr);
        std::string number = containee.substr(0, containee.find(' '));
        if(number == "no")
            break;
        int count = std::stoi(number);
        std::string bag = containee.substr(containee.find(' ') + 1);
        if(count == 1){
            bag = bag + "s";
        }
        containees.push_back(std::make_pair(bag, count));
    }
    
    //std::cout << "\"" << container << "\"" << std::endl;
    if(names_to_ids.find(container) == names_to_ids.end()){
        Bag* next = new Bag(container);
        bags.push_back(next);
        names_to_ids.emplace(next->kind, next->bid);
    }
    Bag* container_bag = bags[names_to_ids[container]];
    for(int i = 0; i < containees.size(); i++){
        if(names_to_ids.find(containees[i].first) == names_to_ids.end()){
            Bag* next = new Bag(containees[i].first);
            bags.push_back(next);
            names_to_ids.emplace(next->kind, next->bid);
        }
        Bag* contained_bag = bags[names_to_ids[containees[i].first]];
        Edge connection;
        connection.amount = containees[i].second;
        connection.containee = contained_bag;
        connection.container = container_bag;
        container_bag->contains.push_back(connection);
        contained_bag->contained_by.push_back(connection);
    }
    //rules.emplace(container, containees);
}

void print_contained_tree(Bag* bag, int level, std::unordered_set<int>& containing_ids){
    for(int i = 0; i < level; i++){
        std::cout << " ";
    }
    std::cout << bag->kind << std::endl;
    for(int i = 0; i < bag->contained_by.size(); i++){
        containing_ids.insert(bag->contained_by[i].container->bid);
        print_contained_tree(bag->contained_by[i].container, level + 1, containing_ids);
    }
}

int get_number_contained_in(Bag* bag){
    int amount = 1;
    for(int i = 0; i < bag->contains.size(); i++){
        amount += bag->contains[i].amount * get_number_contained_in(bag->contains[i].containee);
    }
    return amount;
}

int main(){
    std::ifstream input;
    input.open("input.txt", std::ifstream::in);
    
    std::vector<Bag*> bags;
    std::unordered_map<std::string, int> bag_names_to_ids;
    std::string line;
    std::getline(input, line);
    while(input.good()){
        parse_rule(line, bags, bag_names_to_ids);
        std::getline(input, line);
    }
    std::cout << "shiny gold bags: id " << bag_names_to_ids["shiny gold bags"] << std::endl;
    std::cout << "Bags created: " << bags.size() << std::endl;
    Bag* curr = bags[bag_names_to_ids["shiny gold bags"]];
    std::unordered_set<int> containing_ids;
    print_contained_tree(curr, 0, containing_ids);
    std::cout << containing_ids.size() << " possible containers" << std::endl;
    std::cout << get_number_contained_in(curr) - 1 << " other bags in this bag" << std::endl;
    return 0;
}