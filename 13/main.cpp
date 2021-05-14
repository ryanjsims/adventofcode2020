#include <iostream>
#include <fstream>
#include <vector>

void extended_gcd(uint64_t a, uint64_t b, uint64_t* out){
    int old_r = a;
    int r = b;
    int old_s = 1;
    int s = 0;
    int old_t = 0;
    int t = 1;
    while(r > 0){
        int quotient = old_r / r;
        int temp = r;
        r = old_r - quotient * r;
        old_r = temp;
        temp = s;
        s = old_s - quotient * s;
        old_s = temp;
        temp = t;
        t = old_t - quotient * t;
        old_t = temp; 
    }
    out[0] = old_r;
    out[1] = old_s;
    out[2] = old_t;
}

void combine_phased_rotations(uint64_t a_period, uint64_t a_phase, uint64_t b_period, uint64_t b_phase, uint64_t* out_period, uint64_t* out_phase){
    uint64_t results[3];
    extended_gcd(a_period, b_period, results);
    uint64_t phase_difference = a_phase - b_phase;
    uint64_t pd_mult = phase_difference / results[0];
    uint64_t pd_remainder = phase_difference % results[0];
    uint64_t combined_period = a_period / results[0] * b_period;
    uint64_t combined_phase = (a_phase - results[1] * pd_mult * a_period) % combined_period;
    std::cout << combined_period << " " << combined_phase << std::endl;
    *out_period = combined_period;
    *out_phase = combined_phase;
}

int main(){
    std::ifstream input;
    input.open("input.txt", std::ifstream::in);

    std::string line;
    std::getline(input, line);
    std::getline(input, line);
    int index = 0;
    std::vector<std::pair<uint64_t, uint64_t>> buses;
    int offset = 0;
    while(line.find(',', index) != std::string::npos){
        std::string bus_str = line.substr(index, line.find(',', index) - index);
        if(bus_str != "x"){
            int bus_id = std::stoi(bus_str);
            buses.push_back(std::make_pair(bus_id, offset));
        }
        offset++;
        index = line.find(',', index) + 1;
    }
    std::string bus_str = line.substr(index);
    if(bus_str != "x"){
        int bus_id = std::stoi(bus_str);
        buses.push_back(std::make_pair(bus_id, offset));
    }

    uint64_t timestamp = 0;
    uint64_t increment = 1;
    for(int i = 0; i < buses.size(); i++){
        std::cout << "Bus " << buses[i].first << " must arrive " << buses[i].second << " minutes after the first bus" << std::endl;
        while((timestamp + buses[i].second) % buses[i].first != 0){
            timestamp += increment;
        }
        increment *= buses[i].first;
    }
    std::cout << "Timestamp: " << timestamp << std::endl;
    return 0;
}