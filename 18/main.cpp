#include <iostream>
#include <fstream>
#include <vector>

enum class operation {
    ADD,
    MUL,
    VALUE
};

size_t find_nested_end(std::string expr, size_t index = 0, bool print = false);

class expr_tree {
public:
    expr_tree(std::string expr){
        if(expr[0] == '(' && find_nested_end(expr) == expr.size()){
            expr = expr.substr(1, expr.size() - 2);
        }
        size_t index = expr.find_first_of("(*+");
        if(index == std::string::npos){
            if(expr.size() != 1){
                std::cout << expr << std::endl;
            }
            value = std::stoul(expr);
            type = operation::VALUE;
            left = nullptr;
            right = nullptr;
            return;
        }

        int nest = 0;
        int lowest_index;
        int currpriority = INT32_MAX;
        for(int i = 0; i < expr.size(); i++){
            if(expr[i] == '('){
                nest++;
            } else if(expr[i] == ')'){
                nest--;
            } else if((expr[i] == '+' || expr[i] == '*') && priority(expr[i], nest) < currpriority){
                lowest_index = i;
                currpriority = priority(expr[i], nest);
                if(currpriority == 0)
                    break;
            }
        }

        if(expr[lowest_index] == '*'){
            type = operation::MUL;
        } else {
            type = operation::ADD;
        }
        left = new expr_tree(expr.substr(0, lowest_index - 1));
        right = new expr_tree(expr.substr(lowest_index + 2));
    }

    ~expr_tree(){
        if(left){
            delete left;
            left = nullptr;
        }
        if(right){
            delete right;
            right = nullptr;
        }
    }

    uint64_t eval(){
        switch(type){
        case operation::ADD:
            return left->eval() + right->eval();
        case operation::MUL:
            return left->eval() * right->eval();
        case operation::VALUE:
            return value;
        }
        return -1;
    }

    void print(int level = 0){
        for(int i = 0; i < level; i++){
            std::cout << "    ";
        }
        switch(type){
        case operation::ADD:
            std::cout << "+" << std::endl;
            left->print(level+1);
            right->print(level+1);
            break;
        case operation::MUL:
            std::cout << "*" << std::endl;
            left->print(level+1);
            right->print(level+1);
            break;
        case operation::VALUE:
            std::cout << value << std::endl;
            break;
        }
    }
private:
    expr_tree *left, *right;
    operation type;
    uint64_t value;

    int priority(char op, int nesting){
        if(op == '+'){
            return ((nesting << 1) + 1);
        }
        return nesting << 1;
    }
};

size_t find_nested_end(std::string expr, size_t index, bool print){
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
    return index;
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
    operation op = operation::VALUE;
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
            case operation::VALUE:
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
        expr_tree *e = new expr_tree(line);
        //std::cout << line << std::endl;
        //e->print();
        //std::cout << e->eval() << std::endl;
        sum += e->eval();
        delete e;
        std::getline(input, line);
    }
    std::cout << sum << std::endl;

    return 0;
}