#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>

enum class Seating {
    EMPTY,
    OCCUPIED,
    FLOOR
};

int count_occupied(std::vector<std::vector<Seating>> floorplan, int r, int c){
    int total = 0;
    for(int i = std::max(r - 1, 0); i < std::min(r + 2, (int)floorplan.size()); i++){
        for(int j = std::max(c - 1, 0); j < std::min(c + 2, (int)floorplan[r].size()); j++){
            if(i == r && j == c)
                continue;
            if(floorplan[i][j] == Seating::OCCUPIED){
                total++; 
            }
        }
    }
    return total;
}

int raycast_occupied(std::vector<std::vector<Seating>> floorplan, int r, int c){
    int total = 0;
    std::pair<int, int> directions[8] = {
        std::make_pair(-1, -1),
        std::make_pair(-1, 0),
        std::make_pair(-1, 1),
        std::make_pair(0, -1),
        std::make_pair(0, 1),
        std::make_pair(1, -1),
        std::make_pair(1, 0),
        std::make_pair(1, 1)
    };
    for(int idx = 0; idx < 8; idx++){
        int i = r + directions[idx].first;
        int j = c + directions[idx].second;
        while(i >= 0 && j >= 0 && i < floorplan.size() && j < floorplan[i].size()){
            if(i == r && j == c)
                continue;
            if(floorplan[i][j] == Seating::OCCUPIED){
                total++; 
                break;
            } else if (floorplan[i][j] == Seating::EMPTY){
                break;
            }
            i += directions[idx].first;
            j += directions[idx].second;
        }
    }
    return total;
}

void draw_row(std::vector<Seating> row){
    for(int c = 0; c < row.size(); c++){
        switch(row[c]){
        case Seating::EMPTY:
            std::cout << "L";
            break;
        case Seating::OCCUPIED:
            std::cout << "#";
            break;
        case Seating::FLOOR:
            std::cout << ".";
            break;
        }
    }
}

int main(){
    std::ifstream input;
    input.open("input.txt", std::ifstream::in);

    std::vector<std::vector<Seating>> floorplan1, floorplan2;
    std::string line;
    std::getline(input, line);
    while(input.good()){
        std::vector<Seating> row1, row2;
        for(int i = 0; i < line.size(); i++){
            switch(line[i]){
            case 'L':
                row1.push_back(Seating::EMPTY);
                row2.push_back(Seating::EMPTY);
                break;
            case '.':
                row1.push_back(Seating::FLOOR);
                row2.push_back(Seating::FLOOR);
                break;
            case '#':
                row1.push_back(Seating::OCCUPIED);
                row2.push_back(Seating::OCCUPIED);
                break;
            }
        }
        floorplan1.push_back(row1);
        floorplan2.push_back(row2);

        std::getline(input, line);
    }
    std::vector<std::vector<Seating>>* current = &floorplan1, *output = &floorplan2;
    int changes;
    do {
        changes = 0;
        for(int r = 0; r < current->size(); r++){
            for(int c = 0; c < (*current)[r].size(); c++){
                if((*current)[r][c] == Seating::FLOOR){
                    continue;
                }
                int occupied = count_occupied(*current, r, c);
                if((*current)[r][c] == Seating::EMPTY && occupied == 0){
                    (*output)[r][c] = Seating::OCCUPIED;
                    changes++;
                } else if((*current)[r][c] == Seating::OCCUPIED && occupied >= 4){
                    (*output)[r][c] = Seating::EMPTY;
                    changes++;
                } else {
                    (*output)[r][c] = (*current)[r][c];
                }
            }
            //draw_row((*current)[r]);
            //std::cout << "\t";
            //draw_row((*output)[r]);
            //std::cout << std::endl;
        }
        //std::cout << std::endl;
        std::swap(current, output);
    } while(changes != 0);
    int total_occupied = 0;
    for(int r = 0; r < current->size(); r++){
        for(int c = 0; c < (*current)[r].size(); c++){
            if((*current)[r][c] == Seating::OCCUPIED){
                total_occupied++;
            }
        }
    }
    std::cout << "Total occupied upon stabilization: " << total_occupied << std::endl;
    return 0;
}