#include <fstream>
#include <vector>
#include <iostream>

int main(int argc, char** argv){
    std::ifstream input;
    input.open("input.txt", std::ifstream::in);

    std::vector<int> expenses;
    char line[10];

    while(input.good()){
        input.getline(line, sizeof(line));
        expenses.push_back(atoi(line));
    }
    input.close();

    int i = 0;
    int j = 1;
    int k = 2;
    while(expenses[i] + expenses[j] + expenses[k] != 2020){
        k++;
        if (k == expenses.size()){
            j++;
            if(j == expenses.size() - 1){
                i++;
                j = i + 1;
            }
            k = j + 1;
        }
        //std::cout << i << std::endl;
        //std::cout << j << std::endl;
        //std::cout << k << std::endl;

    }
    std::cout << "The three entries that sum to 2020 are " << expenses[i] << ", " << expenses[j] << " and " << expenses[k] << std::endl;
    std::cout << "They multiply to " << expenses[i] * expenses[j] * expenses[k] << std::endl;
    return 0;
}