#include <iostream>
#include <fstream>
#include <unordered_map>
#include <unordered_set>

int main(int argc, char** argv){
    std::string filename = "input.txt";
    std::ifstream input;
    if(argc == 2){
        filename = argv[1];
    }
    input.open(filename, std::ifstream::in);

    std::string line;
    std::unordered_map<std::string, std::unordered_set<std::string>> ingredients_with;
    std::unordered_map<std::string, int> all_ingredients;
    size_t index = 0;
    while(input.good()){
        std::getline(input, line);
        if(line.size() == 0)
            break;
        int start = line.find("contains ") + 9;
        std::string allergens = line.substr(start, line.find(")") - start);
        std::string ingredients = line.substr(0, line.find(" ("));
        std::unordered_set<std::string> potential_allergens;
        while(index != std::string::npos){
            if(index != 0)
                index++;
            std::string ingredient = ingredients.substr(index, ingredients.find(" ", index) - index);
            index = ingredients.find(" ", index);
            potential_allergens.insert(ingredient);
            if(all_ingredients.find(ingredient) == all_ingredients.end()){
                all_ingredients.emplace(ingredient, 0);
            }
            all_ingredients[ingredient]++;
        }
        index = 0;
        while(index != std::string::npos){
            if(index != 0)
                index += 2;
            std::string allergen = allergens.substr(index, allergens.find(", ", index) - index);
            index = allergens.find(", ", index);
            auto it = ingredients_with.find(allergen);
            if(it == ingredients_with.end()){
                ingredients_with[allergen] = potential_allergens;
            } else {
                for(auto it2 = it->second.begin(); it2 != it->second.end(); ){
                    if(potential_allergens.find(*it2) == potential_allergens.end()){
                        it2 = it->second.erase(it2);
                    } else {
                        it2++;
                    }
                }
            }
        }
        index = 0;
    }
    int i = 0;
    for(auto it = all_ingredients.begin(); it != all_ingredients.end(); it++){
        bool no_allergen = true;
        for(auto it2 = ingredients_with.begin(); it2 != ingredients_with.end(); it2++){
            if(it2->second.find(it->first) != it2->second.end()){
                no_allergen = false;
                break;
            }
        }
        if(no_allergen){
            i += it->second;
        }
    }
    std::cout << i << std::endl;
    int total = 0;
    for(auto it = all_ingredients.begin(); it != all_ingredients.end(); it++){
        total += it->second;
    }
    std::cout << total << std::endl;

    return 0;
}