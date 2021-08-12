#include <iostream>
#include <fstream>
#include <deque>
#include <unordered_set>
#include <sstream>

#ifdef USEGL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <opengl/window.hpp>
#include <opengl/shader.hpp>
#include <opengl/mesh.hpp>

#include "card.hpp"
#endif

#ifndef DEBUG
#define DEBUG 0
#endif

std::string base58 = "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";

std::string build_state(std::deque<int> p1, std::deque<int> p2){
    std::stringstream builder;
    for(int i = 0; i < p1.size(); i++){
        builder << base58[p1[i]];
    }
    builder << '-';
    for(int i = 0; i < p2.size(); i++){
        builder << base58[p2[i]];
    }
    std::string to_return = builder.str();
    builder.clear();
    return to_return;
}

bool play_round_recursive(std::deque<int> p1, std::deque<int> p2, int current_game);

int game = 1;

void print_deck(std::deque<int> deck){
    for(int i = 0; i < deck.size(); i++){
        if(i != 0){
            std::cout << ", ";
        }
        std::cout << deck[i];
    }
    std::cout << std::endl;
}

//True if p1 wins, false if p2 wins
bool play_recursive(std::deque<int>& p1, std::deque<int>& p2){
    std::unordered_set<std::string> game_log;
    int current_game = game;
    game++;
    int round = 1;
    #if DEBUG > 1
    std::cout << "=== Game " << current_game << " ===\n";
    #endif
    while(p1.size() > 0 && p2.size() > 0){
        std::string hash = build_state(p1, p2);
        #if DEBUG > 4
        std::cout << "Adding game state " << hash << " to log." << std::endl;
        #endif
        
        #if DEBUG > 2
        std::cout << "\n-- Round " << round << " (Game " << current_game << ") --" << std::endl;
        std::cout << "Player 1's deck: ";
        print_deck(p1);
        std::cout << "Player 2's deck: ";
        print_deck(p2);
        std::cin.get();
        #endif
        
        if(game_log.find(hash) != game_log.end()){
            #if DEBUG > 3
            std::cout << "The winner of game " << current_game << " is player 1 due to previously seen game state!\n";
            std::cout << hash << std::endl << std::endl;
            std::cin.get();
            #endif
            return true;
        }
        game_log.insert(hash);

        #if DEBUG > 2
        std::cout << "Player 1 plays: " << p1.front() << std::endl;
        std::cout << "Player 2 plays: " << p2.front() << std::endl;
        #endif

        bool winner = play_round_recursive(p1, p2, current_game);
        int card1 = p1.front();
        int card2 = p2.front();
        p1.pop_front();
        p2.pop_front();
        if(winner){
            p1.push_back(card1);
            p1.push_back(card2);
        } else {
            p2.push_back(card2);
            p2.push_back(card1);
        }
        #if DEBUG > 2
        std::cout << "Player " << (winner ? 1 : 2) << " wins round " << round << " of game " << current_game << "!" << std::endl;
        #endif
        round++;
    }
    #if DEBUG > 1
    std::cout << "The winner of game " << current_game << " is player " << (p1.size() != 0 ? 1 : 2) << "!" << std::endl << std::endl;
    #endif
    game_log.clear();
    return p1.size() != 0;
}

bool play_round_recursive(std::deque<int> p1, std::deque<int> p2, int current_game){
    //Note: Using copies of the decks
    int card1 = p1.front();
    p1.pop_front();
    int card2 = p2.front();
    p2.pop_front();
    if(card1 <= p1.size() && card2 <= p2.size()){
        std::deque<int> copy1, copy2;
        for(int i = 0; i < card1; i++){
            copy1.push_back(p1.front());
            p1.pop_front();
        }
        for(int i = 0; i < card2; i++){
            copy2.push_back(p2.front());
            p2.pop_front();
        }
        #if DEBUG > 2
        std::cout << "Playing a sub-game to determine the winner..." << std::endl << std::endl;
        //std::cin.get();
        #endif
        bool to_return = play_recursive(copy1, copy2);
        #if DEBUG > 2
        std::cout << "...anyway, back to game " << current_game << std::endl;
        #endif
        return to_return;
    } else {
        return card1 > card2;
    }
}

bool play_round(std::deque<int>& p1, std::deque<int>& p2){
    int card1 = p1.front();
    p1.pop_front();
    int card2 = p2.front();
    p2.pop_front();
    if(card1 > card2){
        p1.push_back(card1);
        p1.push_back(card2);
    } else if(card2 > card1){
        p2.push_back(card2);
        p2.push_back(card1);
    } else {
        std::cerr << "ERROR: Tie? " << card1 << " == " << card2 << std::endl;
        exit(EXIT_FAILURE);
    }
    return card1 > card2;
}

bool play_game(std::deque<int>& p1, std::deque<int>& p2){
    int round = 1;
    while(p1.size() != 0 && p2.size() != 0){
        bool winner = play_round(p1, p2);
        #if DEBUG > 0
        if(winner){
            std::cout << "Player 1";
        } else {
            std::cout << "Player 2";
        }
        std::cout << " won round " << round << std::endl;
        #endif
        round++;
    }
    return p1.size() != 0;
}

int main(int argc, char** argv){
    std::string filename = "input.txt";
    std::ifstream input;
    bool recursive = false;
    if(argc >= 2){
        filename = argv[1];
    }
    if(argc >= 3){
        recursive = atoi(argv[2]);
    }
    input.open(filename, std::ifstream::in);
    std::deque<int> deck1, deck2, *curr;

    std::string line;
    while(input.good()){
        std::getline(input, line);
        if(line.size() == 0){
            continue;
        } else if(line == "Player 1:"){
            curr = &deck1;
        } else if(line == "Player 2:"){
            curr = &deck2;
        } else {
            curr->push_back(std::stoi(line));
        }
    }
    bool winner;
    #if DEBUG > 0
    std::cout << "== Decks built ==\nPlayer 1's deck: ";
    print_deck(deck1);
    std::cout << "Player 2's deck: ";
    print_deck(deck2);
    std::cout << "\n";
    #endif
    int start_amt = deck1.size() + deck2.size();
    if(recursive){
        winner = play_recursive(deck1, deck2);
    } else {
        winner = play_game(deck1, deck2);
    }
    #if DEBUG > 0
    std::cout << "== Post-game results ==\nPlayer 1's deck: ";
    print_deck(deck1);
    std::cout << "Player 2's deck: ";
    print_deck(deck2);
    std::cout << "\n";
    #endif
    if(deck1.size() + deck2.size() != start_amt){
        std::cout << start_amt << " != " << deck1.size() + deck2.size() << std::endl;
    }
    curr = winner ? &deck1 : &deck2;
    uint64_t score = 0, i = 1;
    while(curr->size() > 0){
        score += curr->back() * i;
        i++;
        curr->pop_back();
    }
    std::cout << "Winner's score: " << score << std::endl;
#ifdef USEGL
    rjs::window window("Combat!", 300, 100, 800, 600);
    rjs::shader shader("shaders/card.vs.glsl", "shaders/card.fs.glsl");
    if(!shader.good()){
        exit(EXIT_FAILURE);
    }
    glActiveTexture(GL_TEXTURE0);
    std::vector<rjs::mesh> card_meshes = rjs::mesh::generate(4);
    std::vector<card> cards;
    for(int i = 0; i < card_meshes.size(); i++){
        cards.push_back(card(i, card_meshes[i]));
    }
    glm::mat4 model, projection, view;
    model = glm::identity<glm::mat4>();
    projection = glm::perspective(glm::radians(74.0f), (float)window.width() / (float)window.height(), 0.1f, 1000.0f);
    view = glm::identity<glm::mat4>();
    view = glm::translate(view, glm::vec3(-2.5f, 0.0f, -5.0f));

    shader.setMat4("view", view);
    shader.setMat4("projection", projection);
    shader.setInt("card_atlas", 0);

    window.set_clear_color(glm::vec4(0.2, 0.2, 0.2, 1.0));
    window.on_draw([&cards, &model, &shader](double delta_time){
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        model = glm::identity<glm::mat4>();
        for(int i = 0; i < cards.size(); i++){
            shader.setMat4("model", model);
            cards[i].draw();
            model = glm::translate(model, glm::vec3(2.5, 0, 0));
        }
    });
    using namespace std::chrono_literals;
    window.run(nullptr, 10ms, true);
#endif
    return 0;
}