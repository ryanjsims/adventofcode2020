#include <iostream>
#include <fstream>
#include <bitset>
#include <array>
#include <vector>
#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>
#include <stb_image.h>
#include <stb_image_write.h>
#include <unordered_map>

#include "opengl/window.hpp"
#include "opengl/shader.hpp"
#include "opengl/texture.hpp"
#include "opengl/vertex.hpp"

#include "tile.hpp"

enum class state {
    PARSE_ID,
    ADD_LINES
};

glm::mat4 view(1.0f), model(1.0f);

void check_errors(){
    int error = glGetError();
    while(error != GL_NO_ERROR){
        std::cout << "GL ERROR: ";
        switch(error){
        case GL_INVALID_ENUM:
            std::cout << "GL_INVALID_ENUM" << std::endl;
            break;
        case GL_INVALID_VALUE:
            std::cout << "GL_INVALID_VALUE" << std::endl;
            break;
        case GL_INVALID_OPERATION:
            std::cout << "GL_INVALID_OPERATION" << std::endl;
            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            std::cout << "GL_INVALID_FRAMEBUFFER_OPERATION" << std::endl;
            break;
        case GL_OUT_OF_MEMORY:
            std::cout << "GL_OUT_OF_MEMORY" << std::endl;
            break;
        default:
            std::cout << "Unknown: " << error << std::endl;
            break;
        }
        error = glGetError();
    }
}

void draw_tiles();

std::vector<std::shared_ptr<tile>> tiles;
std::unordered_map<ulong, std::array<std::shared_ptr<tile>, 2>> borders_to_tiles;
int flip_target = 0;

// void connect(std::shared_ptr<tile> first, std::shared_ptr<tile> second, int k, int l, bool flip_first, bool flip_second, ulong border){
//     std::array<std::bitset<10>, 4> border_i = first->border(), border_j = second->border(flip_second);
//     /*if(flip_second){
//         second->flip();
//         std::cout << "Flipped second" << std::endl;
//     }
//     if(flip_first){
//         first->flip();
//         std::cout << "Flipped first" << std::endl;
//     }*/
//     k = first->get_border_index(border);
//     if(k == -1){
//         k = first->get_border_index(border, true);
//         /*if(first->neighbors() == 0){
//             first->flip();
//             std::cout << "Flipped first" << std::endl;
//         }*/
//     }
//     l = second->get_border_index(border);
//     if(l == -1){
//         l = second->get_border_index(border, true);
//         /*if(second->neighbors() == 0){
//             second->flip();
//             std::cout << "Flipped second" << std::endl;
//         }*/
//     }
//     switch((k - l)){
//     case 0:
//         second->rotate_neighborhood(direction::CCW);
//         second->unfix_neighborhood();
//         second->rotate_neighborhood(direction::CCW);
//         second->unfix_neighborhood();
//         std::cout << "Rotated second twice" << std::endl;
//         break;
//     case -3:
//     case  1:
//         second->rotate_neighborhood(direction::CCW);
//         second->unfix_neighborhood();
//         std::cout << "Rotated second CCW" << std::endl;
//         break;
//     case  3:
//     case -1:
//         second->rotate_neighborhood(direction::CW);
//         second->unfix_neighborhood();
//         std::cout << "Rotated second CW" << std::endl;
//         break;
//     case  2:
//     case -2:
//         break;
//     default:
//         std::cerr << "Somethings wrong with your logic, yo!" << std::endl;
//         break;
//     }
//     k = first->get_border_index(border);
//     if(k == -1)
//         k = first->get_border_index(border, true);
//     l = second->get_border_index(border);
//     if(l == -1)
//         l = second->get_border_index(border, true);
//     first->set_neighbor(k, second);
//     second->set_neighbor(l, first);
//     int i = 0, j = 0;
//     for(i = 0; i < tiles.size(); i++){
//         if(first == tiles[i])
//             break;
//     }
//     for(j = 0; j < tiles.size(); j++){
//         if(second == tiles[j])
//             break;
//     }
//     std::cout << "k: " << k << " l: " << l << std::endl;
//     std::cout << "Connected index " << i << ": ID" << first->id << " and index " << j << ": ID" << second->id;
//     switch(k){
//     case 0:
//         std::cout << " top and bottom" << std::endl;
//         break;
//     case 1:
//         std::cout << " right and left" << std::endl;
//         break;
//     case 2:
//         std::cout << " bottom and top" << std::endl;
//         break;
//     case 3:
//         std::cout << " left and right" << std::endl;
//         break;
//     }
// }

void build_from_tile(std::shared_ptr<tile> curr, int level = 0){
    if(curr->fixed){
        return;
    }
    //std::cout << "Linking " << curr->id << std::endl;
    auto borders = curr->border();
    for(int i = 0; i < borders.size(); i++){
        auto it = borders_to_tiles.find(borders.at(i).to_ulong());
        if(it == borders_to_tiles.end()){
            continue;
        }
        std::array<std::shared_ptr<tile>, 2> neighbors = it->second;
        if(neighbors[1] == nullptr){
            borders_to_tiles.erase(curr->border().at(i).to_ulong());
            borders_to_tiles.erase(curr->border(true).at(i).to_ulong());
            continue;
        }
        std::shared_ptr<tile> neighbor = neighbors[0]->id == curr->id ? neighbors[1] : neighbors[0];
        if(neighbor->neighbors() != 0){
            continue;
        }
        int border_direction = -1;
        auto other_borders = neighbor->border();
        for(int j = 0; j < other_borders.size(); j++){
            if(other_borders[j].to_ulong() == borders.at(i).to_ulong()){
                border_direction = j;
            }
        }
        //std::cout << border_direction << std::endl;
        if(border_direction == -1){
            neighbor->hflip();
            for(int i = 0; i < level; i++){
                std::cout << "    ";
            }
            std::cout << "Hflipped " << neighbor->id << std::endl;
            other_borders = neighbor->border();
            for(int j = 0; j < other_borders.size(); j++){
                if(other_borders[j].to_ulong() == borders.at(i).to_ulong()){
                    border_direction = j;
                }
            }
            if(border_direction == -1){
                neighbor->vflip();
                neighbor->hflip();
                std::cout << "Hflipped " << neighbor->id << std::endl;
                other_borders = neighbor->border();
                for(int j = 0; j < other_borders.size(); j++){
                    if(other_borders[j].to_ulong() == borders.at(i).to_ulong()){
                        border_direction = j;
                    }
                }
            }
        }
        if(border_direction == i){
            neighbor->rotate(direction::CCW);
            neighbor->rotate(direction::CCW);
        } else if(border_direction - i == 1 || border_direction - i == -3){
            neighbor->rotate(direction::CW);
        } else if(border_direction - i == -1 || border_direction - i == 3){
            neighbor->rotate(direction::CCW);
        }
        other_borders = neighbor->border();
        for(int j = 0; j < other_borders.size(); j++){
            if(other_borders[j].to_ulong() == borders.at(i).to_ulong()){
                border_direction = j;
            }
        }
        if(glm::abs(border_direction - i) != 2){
            std::cerr << "Unexpected border direction! " << border_direction << " " << i << std::endl;
            exit(1);
        }
        curr->set_neighbor(i, neighbor);
        neighbor->set_neighbor(border_direction, curr);
        for(int i = 0; i < level; i++){
            std::cout << "    ";
        }
        std::cout << "Connected " << curr->id << " and " << neighbor->id << " as ";
        switch(i){
        case 0:
            std::cout << "bottom";
            break;
        case 1:
            std::cout << "left";
            break;
        case 2:
            std::cout << "top";
            break;
        case 3:
            std::cout << "right";
            break;
        }

        std::cout << " and ";

        switch(border_direction){
        case 0:
            std::cout << "bottom";
            break;
        case 1:
            std::cout << "left";
            break;
        case 2:
            std::cout << "top";
            break;
        case 3:
            std::cout << "right";
            break;
        }
        std::cout << std::endl;
        //borders_to_tiles.erase(curr->border(false).at(i).to_ulong());
        //borders_to_tiles.erase(curr->border(true).at(i).to_ulong());
        //std::cout << borders_to_tiles.size() << std::endl;
    }
    curr->fixed = true;
    if(curr->get_top()){
        for(int i = 0; i < level; i++){
            std::cout << "    ";
        }
        std::cout << "Building top" << std::endl;
        build_from_tile(curr->get_top(), level + 1);
        for(int i = 0; i < level; i++){
            std::cout << "    ";
        }
        std::cout << "Done" << std::endl;
    }
    if(curr->get_right()){
        for(int i = 0; i < level; i++){
            std::cout << "    ";
        }
        std::cout << "Building right" << std::endl;
        build_from_tile(curr->get_right(), level + 1);
        for(int i = 0; i < level; i++){
            std::cout << "    ";
        }
        std::cout << "Done" << std::endl;
    }
    if(curr->get_bottom()){
        for(int i = 0; i < level; i++){
            std::cout << "    ";
        }
        std::cout << "Building bottom" << std::endl;
        build_from_tile(curr->get_bottom(), level + 1);
        for(int i = 0; i < level; i++){
            std::cout << "    ";
        }
        std::cout << "Done" << std::endl;
    }
    if(curr->get_left()){
        for(int i = 0; i < level; i++){
            std::cout << "    ";
        }
        std::cout << "Building left" << std::endl;
        build_from_tile(curr->get_left(), level + 1);
        for(int i = 0; i < level; i++){
            std::cout << "    ";
        }
        std::cout << "Done" << std::endl;
    }
}

std::shared_ptr<tile> find_match(ulong border, int current_id, int* border_index, bool* hflip, bool* vflip){
    std::array<std::shared_ptr<tile>, 2> match = borders_to_tiles[border];
    std::shared_ptr<tile> top = match[0]->id == current_id ? match[1] : match[0];
    if(top == nullptr)
        return nullptr;
    *border_index = -1;
    *hflip = false, *vflip = false;
    std::array<std::bitset<10>, 4> borders = top->border();
    for(int j = 0; j < 4 && *border_index == -1; j++){
        std::string temp = borders[j].to_string();
        std::reverse(temp.begin(), temp.end());
        std::bitset<10> reversed(temp);
        if(borders[j].to_ulong() == border){
            *border_index = j;
            return top;
        } else if(reversed.to_ulong() == border){
            *border_index = j;
            *hflip = (j % 2 == 0);
            *vflip = (j % 2 == 1);
            return top;
        }
    }
    if(*border_index == -1){
        std::cout << "Did not find border " << std::bitset<10>(border) << std::endl;
    }
    return top;
}

void attach_top_and_right(int curr_index){
    std::shared_ptr<tile> top, right;
    ulong curr_top_border = tiles[curr_index]->border()[0].to_ulong();
    ulong curr_right_border = tiles[curr_index]->border()[1].to_ulong();
    int border_index;
    bool hflip, vflip;
    top = find_match(curr_top_border, tiles[curr_index]->id, &border_index, &hflip, &vflip);
    if(top){
        if(hflip){
            top->hflip();
        }
        if(vflip){
            top->vflip();
        }
        switch(border_index){
        case 0:
            top->rotate(direction::CCW);
            top->rotate(direction::CCW);
            break;
        case 1:
            top->rotate(direction::CW);
            break;
        case 3:
            top->rotate(direction::CCW);
            break;
        default:
            break;
        }
        if(top->border()[2].to_ulong() != tiles[curr_index]->border()[0].to_ulong()){
            top->hflip();
        }
    }
    tiles[curr_index]->set_top(top);
    if(top){
        top->set_bottom(tiles[curr_index]);
    }
    right = find_match(curr_right_border, tiles[curr_index]->id, &border_index, &hflip, &vflip);
    if(right){
        if(hflip){
            right->hflip();
        }
        if(vflip){
            right->vflip();
        }
        switch(border_index){
        case 1:
            right->rotate(direction::CCW);
            right->rotate(direction::CCW);
            break;
        case 2:
            right->rotate(direction::CW);
            break;
        case 0:
            right->rotate(direction::CCW);
            break;
        default:
            break;
        }

        if(right->border()[3].to_ulong() != tiles[curr_index]->border()[1].to_ulong()){
            right->vflip();
        }
    }
    tiles[curr_index]->set_right(right);
    if(right){
        right->set_left(tiles[curr_index]);
    }
}

int main(int argc, char** argv){
    std::string filename = "input.txt";
    std::ifstream input;
    int start_id = -1;
    if(argc >= 2){
        filename = argv[1];
    } 
    if(argc >= 3){
        start_id = atoi(argv[2]);
    }
    input.open(filename, std::ifstream::in);

    std::string line;
    std::vector<std::string> tile_lines;
    int id, index;
    state parse_state = state::PARSE_ID;
    std::shared_ptr<tile> start = nullptr;
    while(input.good()){
        std::getline(input, line);
        switch(parse_state){
        case state::PARSE_ID:
            if(line.size() == 0)
                continue;
            index = line.find_first_of("0123456789");
            id = std::stoi(line.substr(index, line.find(":") - index));
            parse_state = state::ADD_LINES;
            break;
        case state::ADD_LINES:
            if(line.size() == 0){
                std::shared_ptr<tile> to_add = std::make_shared<tile>(tile(id, tile_lines));
                if(to_add == nullptr){
                    std::cerr << "Failed to allocate memory" << std::endl;
                    exit(1);
                }
                std::array<std::bitset<10>, 4> border, flipped_border;
                border[0] = to_add->top(false, false);
                border[1] = to_add->right(false, false);
                border[2] = to_add->bottom(false, false);
                border[3] = to_add->left(false, false);
                flipped_border[0] = to_add->top(true, false);
                flipped_border[1] = to_add->right(false, true);
                flipped_border[2] = to_add->bottom(true, false);
                flipped_border[3] = to_add->left(false, true);

                for(int i = 0; i < border.size(); i++){
                    auto it = borders_to_tiles.find(border[i].to_ulong());
                    auto rit = borders_to_tiles.find(flipped_border[i].to_ulong());
                    if(it == borders_to_tiles.end()){
                        borders_to_tiles.emplace(border[i].to_ulong(), std::array<std::shared_ptr<tile>, 2>{to_add, nullptr});
                    } else {
                        it->second[1] = to_add;
                        it->second[0]->add_match();
                        to_add->add_match();
                    }

                    if(rit == borders_to_tiles.end()){
                        borders_to_tiles.emplace(flipped_border[i].to_ulong(), std::array<std::shared_ptr<tile>, 2>{to_add, nullptr});
                    } else {
                        rit->second[1] = to_add;
                    }
                }
                
                tiles.push_back(to_add);
                parse_state = state::PARSE_ID;
                tile_lines.clear();
                break;
            }
            tile_lines.push_back(line);
            break;
        }
    }
    uint64_t result = 1;
    int start_index = 0;
    for(int i = 0; i < tiles.size(); i++){
        if(tiles[i]->is_corner()){
            if(tiles[i]->id == start_id || start_id == -1)
                start_index = i;
            result *= tiles[i]->id;
            std::cout << tiles[i]->id << std::endl;
        }
    }
    std::cout << result << std::endl;
    
    //Orient start tile in bottom left
    ulong top_border = tiles[start_index]->border()[0].to_ulong();
    auto top_it = borders_to_tiles.find(top_border);
    ulong right_border = tiles[start_index]->border()[1].to_ulong();
    auto right_it = borders_to_tiles.find(right_border);
    while(top_it->second[1] == nullptr || right_it->second[1] == nullptr){
        tiles[start_index]->rotate(direction::CW);

        top_border = tiles[start_index]->border()[0].to_ulong();
        top_it = borders_to_tiles.find(top_border);

        right_border = tiles[start_index]->border()[1].to_ulong();
        right_it = borders_to_tiles.find(right_border);
    }
    int curr_index = start_index, line_start = start_index;

    //int atlas_row_stride = 8 * 4 * glm::sqrt(tiles.size());
    for(int i = 0; i < tiles.size(); i++){
        attach_top_and_right(curr_index);
        //tiles[curr_index]->load_texture_to_atlas_transformed(transformed, (tiles_edge - y - 1) * 8, atlas_row_stride, x * 8, false, true);
        if(tiles[curr_index]->get_right()){
            curr_index = std::find(tiles.begin(), tiles.end(), tiles[curr_index]->get_right()) - tiles.begin();
        } else if(tiles[line_start]->get_top()) {
            curr_index = std::find(tiles.begin(), tiles.end(), tiles[line_start]->get_top()) - tiles.begin();
            line_start = curr_index;
        }
    }    
    std::cout << "All rearranged" << std::endl;
    tiles[start_index]->unfix_neighborhood();
    tiles[start_index]->fix_location(0, 0);
    flip_target = start_index;
    draw_tiles();
    return 0;
}

void create_texture_atlas(std::vector<uint>& texture_ids){
    if(texture_ids.size() < 2){
        std::cerr << "Error: 2 texture buffers required for normal and edgeless texture atlases" << std::endl;
        exit(1);
    }
    size_t texture_size = tiles.size() * 400;
    size_t edgeless_size = tiles.size() * (400 - 36);
    unsigned char *atlas = new unsigned char[texture_size];
    unsigned char *edgeless_atlas = new unsigned char[edgeless_size];
    if(atlas == nullptr || edgeless_atlas == nullptr){
        std::cerr << "Could not allocate memory for texture atlas!" << std::endl;
        return;
    }
    int x = 0;
    int y = 0;
    int atlas_row_stride = 10 * 4 * glm::sqrt(tiles.size());
    int edge_row_stride = 8 * 4 * glm::sqrt(tiles.size());
    int atlas_tiles = glm::sqrt(tiles.size());

    for(int i = 0; i < tiles.size(); i++){
        tiles[i]->load_texture_to_atlas(atlas, y * 10, atlas_row_stride, x * 10, false, false);
        tiles[i]->load_texture_to_atlas(edgeless_atlas, y * 8, edge_row_stride, x * 8, false, false, 1, 9);
        std::array<float, 8> tex_coords = {
            (float)(x + 0.999) / (float)atlas_tiles, (float)(y + 0.999) / (float)atlas_tiles,
            (float)(x + 0.001) / (float)atlas_tiles, (float)(y + 0.999) / (float)atlas_tiles,
            (float)(x + 0.001) / (float)atlas_tiles, (float)(y + 0.001) / (float)atlas_tiles,
            (float)(x + 0.999) / (float)atlas_tiles, (float)(y + 0.001) / (float)atlas_tiles,
        };
        
        
        tiles[i]->set_tex_coords(tex_coords);
        x++;
        if(x == atlas_tiles){
            y++;
            x = 0;
        }
    }
    //stbi_write_png("edgeless.png", 8 * glm::sqrt(tiles.size()), 8 * glm::sqrt(tiles.size()), 4, edgeless_atlas, edge_row_stride);

    glBindTexture(GL_TEXTURE_2D, texture_ids[0]);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 10 * atlas_tiles, 10 * atlas_tiles, 0, GL_RGBA, GL_UNSIGNED_BYTE, atlas);

    glBindTexture(GL_TEXTURE_2D, texture_ids[1]);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 8 * atlas_tiles, 8 * atlas_tiles, 0, GL_RGBA, GL_UNSIGNED_BYTE, edgeless_atlas);
    delete[] atlas;
    delete[] edgeless_atlas;
}

float viewx = -6, viewy = -6, viewz = -10;
std::unordered_map<uint, bool> keys_down;
std::vector<uint> texture_ids;
int bound_texture = 0;

void draw_tiles(){
    rjs::window window("Tile Mapping", 300, 100, 800, 600);
    
    rjs::shader shader("./shaders/tile.vs.glsl", "./shaders/tile.fs.glsl");
    if(!shader.good()){
        return;
    }
    shader.use();
    texture_ids = rjs::texture::generate(2);
    uint *VAOs = new uint[tiles.size()];
    uint *VBOs = new uint[tiles.size()];
    uint *EBOs = new uint[tiles.size()];

    glGenVertexArrays(tiles.size(), VAOs);
    glGenBuffers(tiles.size(), VBOs);
    glGenBuffers(tiles.size(), EBOs);

    glActiveTexture(GL_TEXTURE0);
    create_texture_atlas(texture_ids);
    glBindTexture(GL_TEXTURE_2D, texture_ids[0]);
    
    int x = 0, y = 0;
    for(int i = 0; i < tiles.size(); i++){
        tiles[i]->load_vertices(VAOs[i], VBOs[i], EBOs[i], x, y);
        x++;
        if(x == (int)glm::sqrt(tiles.size())){
            x = 0;
            y++;
        }
    }

    std::cout << "Created " << tiles.size() << " tiles" << std::endl;
    glm::mat4 projection(1.0f);
    //projection = glm::ortho(-12.0f, 12.0f, -8.0f, 8.0f, 0.1f, 1000.0f); 
    projection = glm::perspective(glm::radians(74.0f), (float)window.width() / (float)window.height(), 0.1f, 1000.0f);
    
    glm::vec3 selected = glm::vec3(0.0f, 1.0f, 0.8f);
    glm::vec3 deselected = glm::vec3(0.0f, 0.0f, 0.0f);
    shader.setMat4("model", model);
    shader.setMat4("view", view);
    shader.setMat4("projection", projection);
    shader.setInt("tile_texture", 0);
    shader.setVec3("selected", deselected);

    //tiles[0]->flip();
    
    glClearColor(0.2, 0.2, 0.2, 1.0);

    window.on_draw([VAOs, &shader, selected, deselected](double deltaTime){
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        if(keys_down[GLFW_KEY_I]){
            viewy -= 10 * deltaTime;
        }
        if(keys_down[GLFW_KEY_J]){
            viewx += 10 * deltaTime;
        }
        if(keys_down[GLFW_KEY_K]){
            viewy += 10 * deltaTime;
        }
        if(keys_down[GLFW_KEY_L]){
            viewx -= 10 * deltaTime;
        }
        if(keys_down[GLFW_KEY_U]){
            viewz -= 10 * deltaTime;
        }
        if(keys_down[GLFW_KEY_O]){
            viewz += 10 * deltaTime;
        }
        view = glm::identity<glm::mat4>();
        view = glm::translate(view, glm::vec3(viewx, viewy, viewz));
        shader.setMat4("view", view);
        for(int i = 0; i < tiles.size(); i++){
            if(i == flip_target){
                shader.setVec3("selected", selected);
            } else {
                shader.setVec3("selected", deselected);
            }
            if(tiles[i]->translating()){
                tiles[i]->update_translate(deltaTime);
            } else if(tiles[i]->hflipping()){
                tiles[i]->update_hflip(deltaTime);
            } else if(tiles[i]->vflipping()){
                tiles[i]->update_vflip(deltaTime);
            } else if(tiles[i]->rotating()){
                tiles[i]->update_rotate(deltaTime);
            }
            model = glm::identity<glm::mat4>();
            model = glm::translate(model, glm::vec3(tiles[i]->curr_x(), tiles[i]->curr_y(), 0.0f));
            if(tiles[i]->rotating() || tiles[i]->translating()){
                model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.1f));
            }
            model = glm::rotate(model, glm::radians(tiles[i]->get_hflip()), glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::rotate(model, glm::radians(tiles[i]->get_vflip()), glm::vec3(1.0f, 0.0f, 0.0f));
            model = glm::rotate(model, glm::radians(tiles[i]->get_rotate()), glm::vec3(0.0f, 0.0f, 1.0f));
            shader.setMat4("model", model);
            glBindVertexArray(VAOs[i]);
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);
        }
    });

    window.on_input([](GLFWwindow* window, int key, int scancode, int action, int modifiers){
        keys_down[key] = action != GLFW_RELEASE;
        if(key == GLFW_KEY_ESCAPE){
            glfwSetWindowShouldClose(window, 1);
        }
        if(key == GLFW_KEY_F && action == GLFW_PRESS && flip_target < tiles.size()){
            if(modifiers & GLFW_MOD_SHIFT){
                tiles[flip_target]->unfix_neighborhood();
                if(modifiers & GLFW_MOD_CONTROL){
                    tiles[flip_target]->vflip_neighborhood();
                } else {
                    tiles[flip_target]->hflip_neighborhood();
                }
                tiles[flip_target]->unfix_neighborhood();
                tiles[flip_target]->fix_location(tiles[flip_target]->get_x(), tiles[flip_target]->get_y());
            } else {
                if(modifiers & GLFW_MOD_CONTROL){
                    tiles[flip_target]->vflip();
                } else {
                    tiles[flip_target]->hflip();
                }
            }
        }

        if(key == GLFW_KEY_M && action == GLFW_PRESS && flip_target < tiles.size()){
            std::array<std::bitset<96>, 96> picture;
            int curr_index = flip_target, line_start = flip_target, x = 0, y = 0;
            //int atlas_row_stride = 8 * 4 * glm::sqrt(tiles.size());
            int tiles_edge = glm::sqrt(tiles.size());
            for(int i = 0; i < tiles.size(); i++){
                tiles[curr_index]->load_bits_to_atlas_transformed<96, 96>(picture, (tiles_edge - y - 1) * 8, x * 8, false, true, 1, 9);
                if(tiles[curr_index]->get_right()){
                    curr_index = std::find(tiles.begin(), tiles.end(), tiles[curr_index]->get_right()) - tiles.begin();
                    x += 1;
                } else if(tiles[line_start]->get_top()) {
                    curr_index = std::find(tiles.begin(), tiles.end(), tiles[line_start]->get_top()) - tiles.begin();
                    line_start = curr_index;
                    x = 0;
                    y += 1;
                }
            }
            std::array<std::bitset<96>, 3> sea_monster = {
                std::bitset<96>("00000000000000000010"),
                std::bitset<96>("10000110000110000111"),
                std::bitset<96>("01001001001001001000")
            };
            int total_white = 0, total_monster = 0;
            for(int i = 0; i < picture.size(); i++){
                total_white += picture[i].count();
            }
            for(int i = 0; i < picture.size() - 2; i++){
                for(int j = 0; j < picture[i].size(); j++){
                    if(((sea_monster[0] << j) & picture[i]).count() != sea_monster[0].count()){
                        continue;
                    }
                    if(((sea_monster[1] << j) & picture[i + 1]).count() != sea_monster[1].count()){
                        continue;
                    }
                    if(((sea_monster[2] << j) & picture[i + 2]).count() != sea_monster[2].count()){
                        continue;
                    }
                    std::cout << "Found sea monster at (" << i << ", " << j << ")" << std::endl;
                    int minx = (95 - j - 20) / 8, maxx = (95 - j) / 8, miny = (i) / 8, maxy = (i + 3) / 8;
                    std::cout << "Min x: " << minx << std::endl;
                    std::cout << "Max x: " << maxx << std::endl;
                    std::cout << "Min y: " << miny << std::endl;
                    std::cout << "Max y: " << maxy << std::endl;
                    total_monster += sea_monster[0].count() + sea_monster[1].count() + sea_monster[2].count();
                }
            }
            std::cout << "roughness: " << total_white - total_monster << std::endl; 
        }
        
        if(key == GLFW_KEY_R && action == GLFW_PRESS && flip_target < tiles.size()){
            if(modifiers & GLFW_MOD_SHIFT){
                tiles[flip_target]->unfix_neighborhood();
                tiles[flip_target]->rotate_neighborhood((modifiers & GLFW_MOD_CONTROL) ? direction::CW : direction::CCW);
                tiles[flip_target]->unfix_neighborhood();
                tiles[flip_target]->fix_location(tiles[flip_target]->get_x(), tiles[flip_target]->get_y());
            } else {
                tiles[flip_target]->rotate((modifiers & GLFW_MOD_CONTROL) ? direction::CW : direction::CCW);
            }
                
        }
        if(key >= GLFW_KEY_0 && key <= GLFW_KEY_9 && action == GLFW_PRESS){
            flip_target *= 10;
            flip_target += key - GLFW_KEY_0;
            std::cout << flip_target << "\t" << ((flip_target < tiles.size()) ? tiles[flip_target]->id : -1) << "\r" << std::flush;
        }
        if(key == GLFW_KEY_DELETE && action == GLFW_PRESS){
            flip_target = 0;
            std::cout << flip_target << "\t" << ((flip_target < tiles.size()) ? tiles[flip_target]->id : -1) << "   \r" << std::flush;
        }
        if(key == GLFW_KEY_DOWN && action == GLFW_PRESS && flip_target < tiles.size()){
            if(modifiers & GLFW_MOD_SHIFT){
                tiles[flip_target]->unfix_neighborhood();
                tiles[flip_target]->translate_neighborhood(0, -1);
            } else {
                tiles[flip_target]->translate(0, -1);
            }
        }
        if(key == GLFW_KEY_LEFT && action == GLFW_PRESS && flip_target < tiles.size()){
            if(modifiers & GLFW_MOD_SHIFT){
                tiles[flip_target]->unfix_neighborhood();
                tiles[flip_target]->translate_neighborhood(-1, 0);
            } else {
                tiles[flip_target]->translate(-1, 0);
            }
        }
        if(key == GLFW_KEY_UP && action == GLFW_PRESS && flip_target < tiles.size()){
            if(modifiers & GLFW_MOD_SHIFT){
                tiles[flip_target]->unfix_neighborhood();
                tiles[flip_target]->translate_neighborhood(0, 1);
            } else {
                tiles[flip_target]->translate(0, 1);
            }
        }
        if(key == GLFW_KEY_RIGHT && action == GLFW_PRESS && flip_target < tiles.size()){
            if(modifiers & GLFW_MOD_SHIFT){
                tiles[flip_target]->unfix_neighborhood();
                tiles[flip_target]->translate_neighborhood(1, 0);
            } else {
                tiles[flip_target]->translate(1, 0);
            }
        }
        if(key == GLFW_KEY_W && action == GLFW_PRESS && flip_target < tiles.size()){
            if(tiles[flip_target]->get_top()){
                flip_target = std::find(tiles.begin(), tiles.end(), tiles[flip_target]->get_top()) - tiles.begin();
            }
        }
        if(key == GLFW_KEY_A && action == GLFW_PRESS && flip_target < tiles.size()){
            if(tiles[flip_target]->get_left()){
                flip_target = std::find(tiles.begin(), tiles.end(), tiles[flip_target]->get_left()) - tiles.begin();
            }
        }
        if(key == GLFW_KEY_S && action == GLFW_PRESS && flip_target < tiles.size()){
            if(tiles[flip_target]->get_bottom()){
                flip_target = std::find(tiles.begin(), tiles.end(), tiles[flip_target]->get_bottom()) - tiles.begin();
            }
        }
        if(key == GLFW_KEY_D && action == GLFW_PRESS && flip_target < tiles.size()){
            if(tiles[flip_target]->get_right()){
                flip_target = std::find(tiles.begin(), tiles.end(), tiles[flip_target]->get_right()) - tiles.begin();
            }
        }
        if(key == GLFW_KEY_SPACE && action == GLFW_PRESS && flip_target < tiles.size()){
            attach_top_and_right(flip_target);
            tiles[flip_target]->unfix_neighborhood();
            tiles[flip_target]->fix_location(tiles[flip_target]->get_x(), tiles[flip_target]->get_y());
        }
        if(key == GLFW_KEY_ENTER && action == GLFW_PRESS){
            bound_texture = (bound_texture + 1) % texture_ids.size();
            glBindTexture(GL_TEXTURE_2D, texture_ids[bound_texture]);
        }
        if(key == GLFW_KEY_Q && action == GLFW_PRESS && flip_target < tiles.size()){
            std::string directions[4] = {
                "   top",
                " right",
                "bottom",
                "  left"
            };
            tiles[flip_target]->print();
            std::cout << std::endl;
            tiles[flip_target]->state();
            std::cout << std::endl;
            for(int i = 0; i < 4; i++){
                std::cout << "Border " << directions[i];
                //std::cout << " (" << directions[std::abs(i + tiles[flip_target]->orientation) % 4] << ")";
                std::cout << ": " << tiles[flip_target]->border()[i] << std::endl;
            }
            std::cout << std::endl;
        }
        if(key == GLFW_KEY_T && action == GLFW_PRESS && flip_target < tiles.size()){
            auto tdata = tiles[flip_target]->get_transformed();
            for(int i = 0; i < tdata.size(); i++){
                std::cout << tdata[i] << std::endl;
            }
            std::cout << std::endl;
        }
    });

    window.run();
}