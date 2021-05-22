#include <iostream>
#include <fstream>
#include <vector>

enum class operation {
    ADD,
    MUL,
    START
};

size_t find_nested_end(std::string expr, size_t index = 0, bool print = false){
    int prev = index;
    int opens = 0;
    if(print)
        std::cout << expr << std::endl;
    do {
        switch(expr[index]){
        case '(':
            opens++;
            break;
        case ')':
            opens--;
            break;
        }
        if(print){
            std::cout << ((expr[index] == ')' || expr[index] == '(') ? std::to_string(opens) : " ");
        }
        index++;
    } while(opens > 0);
    if(print)
        std::cout << std::endl;
    return index == std::string::npos ? expr.size() : index;
}

uint64_t eval_expression(std::string expr, int level = 0, bool debug = false){
    if(debug){
        for(int i = 0; i < level; i++){
            std::cout << "\t";
        }
        std::cout << "Evaluating " << expr << std::endl;
    }
    std::size_t index = 0;
    uint64_t value = 0, step = 0;
    operation op = operation::START;
    while(index != std::string::npos && index < expr.size()){
        uint64_t nextval;
        switch(expr[index]){
        case '(':
            nextval = eval_expression(expr.substr(index + 1, find_nested_end(expr, index) - index - 2), level + 1, debug);
            break;
        case '+':
            op = operation::ADD;
            break;
        case '*':
            op = operation::MUL;
            break;
        case ')':
        case ' ':
            index = expr.find_first_not_of(" )", index + 1);
            continue;
        default:
            if(debug && !isdigit(expr[index])){
                for(int i = 0; i < level; i++){
                    std::cout << "\t";
                }
                std::cout << expr << std::endl;
                for(int i = 0; i < level; i++){
                    std::cout << "\t";
                }
                for(int i = 0; i < index && i < expr.size(); i++){
                    std::cout << " ";
                }
                std::cout << "^" << std::endl;
            }
            nextval = std::stoul(expr.substr(index, expr.find_first_of(" )", index) - index));
            break;
        }
        if(step % 2 == 0){
            switch(op){
            case operation::ADD:
                if(debug){
                    for(int i = 0; i < level; i++){
                        std::cout << "\t";
                    }
                    std::cout << value << " + " << nextval << " = " << value + nextval << std::endl;
                }
                value += nextval;
                break;
            case operation::MUL:
                if(debug){
                    for(int i = 0; i < level; i++){
                        std::cout << "\t";
                    }
                    std::cout << value << " * " << nextval << " = " << value * nextval << std::endl;
                }
                value *= nextval;
                break;
            case operation::START:
                value = nextval;
                break;
            }
        }
        step++;
        if(expr[index] == '('){
            index = find_nested_end(expr, index);
        } else {
            index = expr.find_first_not_of(" )", index + 1);
        }
    }
    return value;
}

int main(int argc, char** argv){
    std::ifstream input;
    std::string to_open = argc == 2 ? argv[1] : "input.txt";
    input.open(to_open, std::ifstream::in);
    std::vector<int> results;
    std::string line;
    //std::cout << find_nested_end("((2 + 4 * 9) * (6 + 9 * 8 + 6) + 6) + 2 + 4 * 2", 0, true) << std::endl;
    std::getline(input, line);
    uint64_t sum = 0;
    while(input.good()){
        sum += eval_expression(line, 0, true);
        std::getline(input, line);
    }
    std::cout << sum << std::endl;

    return 0;
}