#ifndef TILE_HPP
#define TILE_HPP

#include <algorithm>
#include <array>
#include <bitset>
#include <vector>
#include <iostream>

#include "opengl/vertex.hpp"
#include "opengl/mesh.hpp"

enum class direction {
    CCW,
    CW
};

class tile {
private:
    std::array<std::bitset<10>, 10> data;
    std::array<float, 8> tex_coords;
    std::shared_ptr<tile> top_tile, bottom_tile, left_tile, right_tile;
    bool hflipped, vflipped;
    float target_hflip = 0, current_hflip = 0;
    float target_vflip = 0, current_vflip = 0;
    float target_rotate = 0, current_rotate = 0;
    float target_x = 0, current_x = 0;
    float target_y = 0, current_y = 0;
    int matches = 0;
    unsigned char* texture = nullptr;
public:
    int id, orientation;
    bool fixed;
    
    tile(int id, std::vector<std::string> lines){
        this->id = id;
        for(int i = 0; i < lines.size(); i++){
            data[i] = std::bitset<10>(lines[i], 0, 10, '.', '#');
        }
        fixed = false;
        top_tile = nullptr;
        left_tile = nullptr;
        bottom_tile = nullptr;
        right_tile = nullptr;
        orientation = 0;
        hflipped = false;
        vflipped = false;
    }

    void print(){
        if(hflipped != vflipped){
            print_flipped();
            return;
        }
        for(int i = 0; i < 10; i++){
            std::cout << data[i] << std::endl;
        }
    }

    void print_flipped(){
        for(int i = 0; i < 10 && hflipped; i++){
            std::string to_flip = data[i].to_string();
            std::reverse(to_flip.begin(), to_flip.end());
            std::cout << to_flip << std::endl;
        }
        for(int i = 9; i >= 0 && vflipped; i--){
            std::cout << data[i].to_string() << std::endl;
        }
    }

    void print_both(){
        for(int i = 0; i < 10; i++){
            std::string to_flip = data[i].to_string();
            std::reverse(to_flip.begin(), to_flip.end());
            std::cout << data[i] << "    " << to_flip << std::endl;
        }
    }

    void add_match(){
        matches++;
    }

    bool is_corner(){
        return matches == 2;
    }

    void set_left(std::shared_ptr<tile> left){
        this->left_tile = left;
    }

    void set_right(std::shared_ptr<tile> right){
        this->right_tile = right;
    }

    void set_top(std::shared_ptr<tile> top){
        this->top_tile = top;
    }

    void set_bottom(std::shared_ptr<tile> bottom){
        this->bottom_tile = bottom;
    }

    std::shared_ptr<tile> get_left(){
        return this->left_tile;
    }

    std::shared_ptr<tile> get_right(){
        return this->right_tile;
    }

    std::shared_ptr<tile> get_top(){
        return this->top_tile;
    }

    std::shared_ptr<tile> get_bottom(){
        return this->bottom_tile;
    }

    void set_neighbor(int dir, std::shared_ptr<tile> neighbor){
        switch(dir){
        case 0:
            set_top(neighbor);
            break;
        case 1:
            set_left(neighbor);
            break;
        case 2:
            set_bottom(neighbor);
            break;
        case 3:
            set_right(neighbor);
            break;
        default:
            std::cerr << "Invalid neighbor index: " << dir << std::endl;
            break;
        }
    }

    int get_x(){
        return target_x;
    }

    int get_y(){
        return target_y;
    }

    void fix_location(int x, int y){
        if(fixed)
            return;
        fixed = true;
        translate_to(x, y);
        if(top_tile)
            top_tile->fix_location(x, y + 1);
        if(bottom_tile)
            bottom_tile->fix_location(x, y - 1);
        if(left_tile)
            left_tile->fix_location(x - 1, y);
        if(right_tile)
            right_tile->fix_location(x + 1, y);
    }

    int neighbors(){
        return (top_tile ? 1 : 0) + (left_tile ? 1 : 0) + (right_tile ? 1 : 0) + (bottom_tile ? 1 : 0);
    }

    void unfix_neighborhood(){
        if(!fixed)
            return;
        fixed = false;
        if(top_tile)
            top_tile->unfix_neighborhood();
        if(bottom_tile)
            bottom_tile->unfix_neighborhood();
        if(left_tile)
            left_tile->unfix_neighborhood();
        if(right_tile)
            right_tile->unfix_neighborhood();
    }

    void rotate_neighborhood(direction d){
        if(fixed)
            return;
        fixed = true;
        rotate(d);
        std::shared_ptr<tile> temp;
        if(top_tile)
            top_tile->rotate_neighborhood(d);
        if(bottom_tile)
            bottom_tile->rotate_neighborhood(d);
        if(left_tile)
            left_tile->rotate_neighborhood(d);
        if(right_tile)
            right_tile->rotate_neighborhood(d);
        temp = top_tile;
        switch(d){
        case direction::CW:
            top_tile = left_tile;
            left_tile = bottom_tile;
            bottom_tile = right_tile;
            right_tile = temp;
            break;
        case direction::CCW:
            top_tile = right_tile;
            right_tile = bottom_tile;
            bottom_tile = left_tile;
            left_tile = temp;
            break; 
        }
    }

    void hflip_neighborhood(){
        if(fixed)
            return;
        fixed = true;
        hflip();
        std::shared_ptr<tile> temp;
        if(top_tile)
            top_tile->hflip_neighborhood();
        if(bottom_tile)
            bottom_tile->hflip_neighborhood();
        if(left_tile)
            left_tile->hflip_neighborhood();
        if(right_tile)
            right_tile->hflip_neighborhood();
        temp = left_tile;
        left_tile = right_tile;
        right_tile = temp;
    }

    void vflip_neighborhood(){
        if(fixed)
            return;
        fixed = true;
        vflip();
        std::shared_ptr<tile> temp;
        if(top_tile)
            top_tile->vflip_neighborhood();
        if(bottom_tile)
            bottom_tile->vflip_neighborhood();
        if(left_tile)
            left_tile->vflip_neighborhood();
        if(right_tile)
            right_tile->vflip_neighborhood();
        temp = top_tile;
        top_tile = bottom_tile;
        bottom_tile = temp;
    }

    void translate_neighborhood(int dx, int dy){
        if(fixed)
            return;
        fixed = true;
        translate(dx, dy);
        if(top_tile)
            top_tile->translate_neighborhood(dx, dy);
        if(bottom_tile)
            bottom_tile->translate_neighborhood(dx, dy);
        if(left_tile)
            left_tile->translate_neighborhood(dx, dy);
        if(right_tile)
            right_tile->translate_neighborhood(dx, dy);
    }

    std::bitset<10> top(bool hflipped = false, bool vflipped = false){
        int index = vflipped ? 9 : 0;
        if(hflipped){
            std::string temp = data[index].to_string();
            std::reverse(temp.begin(), temp.end());
            return std::bitset<10>(temp);
        }
        return data[index];
    }

    std::bitset<10> bottom(bool hflipped = false, bool vflipped = false){
        int index = vflipped ? 0 : 9;
        if(hflipped){
            std::string temp = data[index].to_string();
            std::reverse(temp.begin(), temp.end());
            return std::bitset<10>(temp);
        }
        return data[index];
    }

    std::bitset<10> left(bool hflipped = false, bool vflipped = false){
        std::bitset<10> to_return;
        int index = hflipped ? 0 : 9;
        for(int i = 0; i < 10 && !vflipped; i++){
            to_return[i] = data[i][index];
        }
        for(int i = 0; i < 10 && vflipped; i++){
            to_return[9 - i] = data[i][index];
        }
        return to_return;
    }

    std::bitset<10> right(bool hflipped = false, bool vflipped = false){
        std::bitset<10> to_return;
        int index = hflipped ? 9 : 0;
        for(int i = 0; i < 10 && !vflipped; i++){
            to_return[i] = data[i][index];
        }
        for(int i = 0; i < 10 && vflipped; i++){
            to_return[9 - i] = data[i][index];
        }
        return to_return;
    }

    //Gets the border given a specific rotation and flip state
    std::array<std::bitset<10>, 4> border(int orientation, bool flipped){
        std::array<std::bitset<10>, 4> to_return;
        to_return[(orientation + 0) % 4] = top(flipped);
        to_return[(orientation + 1) % 4] = right(flipped);
        to_return[(orientation + 2) % 4] = bottom(flipped);
        to_return[(orientation + 3) % 4] = left(flipped);
        return to_return;
    }

    //Gets the border, optionally flipping from the current flip state
    std::array<std::bitset<10>, 4> border(bool flip){
        std::array<std::bitset<10>, 4> to_return;
        to_return[(orientation + 0) % 4] = top(flip != hflipped);
        to_return[(orientation + 1) % 4] = right(flip != hflipped);
        to_return[(orientation + 2) % 4] = bottom(flip != hflipped);
        to_return[(orientation + 3) % 4] = left(flip != hflipped);
        return to_return;
    }

    //Gets the border based on current rotation and flip state
    std::array<std::bitset<10>, 4> border(){
        std::array<std::bitset<10>, 4> to_return;
        switch(hflipped << 3 | vflipped << 2 | (orientation & 0x03)){
        //NORMAL -------------------------------
        case 0b1110:
        case 0b0000:
            to_return[0] = top(false, false);
            to_return[1] = right(false, true);
            to_return[2] = bottom(false, false);
            to_return[3] = left(false, true);
            break;
        case 0b1111:
        case 0b0001:
            to_return[0] = left(false, false);
            to_return[1] = top(false, false);
            to_return[2] = right(false, false);
            to_return[3] = bottom(false, false);
            break;
        case 0b1100:
        case 0b0010:
            to_return[0] = bottom(true, false);
            to_return[1] = left(false, false);
            to_return[2] = top(true, false);
            to_return[3] = right(false, false);
            break;
        case 0b1101:
        case 0b0011:
            to_return[0] = right(false, true);
            to_return[1] = bottom(true, false);
            to_return[2] = left(false, true);
            to_return[3] = top(true, false);
            break;
        //VFLIPPED ----------------------------
        case 0b0100:
            to_return[0] = top(false, true);
            to_return[1] = right(false, false);
            to_return[2] = bottom(false, true);
            to_return[3] = left(false, false);
            break;
        case 0b0101:
            to_return[0] = right(false, false);
            to_return[1] = bottom(true, true);
            to_return[2] = left(false, false);
            to_return[3] = top(true, true);
            break;
        case 0b0110:
            to_return[0] = bottom(true, true);
            to_return[1] = left(false, true);
            to_return[2] = top(true, true);
            to_return[3] = right(false, true);
            break;
        case 0b0111:
            to_return[0] = left(false, true);
            to_return[1] = top(false, true);
            to_return[2] = right(false, true);
            to_return[3] = bottom(false, true);
            break;
        //HFLIPPED ------------------------------
        case 0b1000:
            to_return[0] = top(true, false);
            to_return[1] = right(true, true);
            to_return[2] = bottom(true, false);
            to_return[3] = left(true, true);
            break;
        case 0b1001:
            to_return[0] = right(true, true);
            to_return[1] = bottom(false, false);
            to_return[2] = left(true, true);
            to_return[3] = top(false, false);
            break;
        case 0b1010:
            to_return[0] = bottom(false, false);
            to_return[1] = left(true, false);
            to_return[2] = top(false, false);
            to_return[3] = right(true, false);
            break;
        case 0b1011:
            to_return[0] = left(true, false);
            to_return[1] = top(true, false);
            to_return[2] = right(true, false);
            to_return[3] = bottom(true, false);
            break;
        }

        
        return to_return;
    }
    
    void state(){
        std::cout << "Orientation: " << orientation << std::endl;
        std::cout << "hflip: " << hflipped << std::endl;
        std::cout << "vflip: " << vflipped << std::endl;
    }

    //Gets the border based on current rotation and specified state
    std::array<std::bitset<10>, 4> border(bool hflipped, bool vflipped){
        std::array<std::bitset<10>, 4> to_return;
        to_return[(orientation + 0) % 4] = top(hflipped, vflipped);
        to_return[(orientation + 1) % 4] = right(hflipped, vflipped);
        to_return[(orientation + 2) % 4] = bottom(hflipped, vflipped);
        to_return[(orientation + 3) % 4] = left(hflipped, vflipped);
        return to_return;
    }

    int get_border_index(ulong border){
        auto borders = this->border();
        for(int i = 0; i < borders.size(); i++){
            if(borders[i].to_ulong() == border){
                return i;
            }
        }
        return -1;
    }

    void set_tex_coords(std::array<float, 8> coords){
        this->tex_coords = coords;
    }

    std::array<unsigned char, 400> get_texture_data(){
        std::array<unsigned char, 400> to_return;
        for(int i = 0; i < data.size(); i++){
            for(int j = 0; j < data[i].size(); j++){
                to_return[i * 40 + j * 4] = ((bool)data[i][j]) ? 255 : 0;
                to_return[i * 40 + j * 4 + 1] = ((bool)data[i][j]) ? 255 : 0;
                to_return[i * 40 + j * 4 + 2] = ((bool)data[i][j]) ? 255 : 0;
                to_return[i * 40 + j * 4 + 3] = 255;
                
            }
        }
        return to_return;
    }

    void load_texture(uint texture){
        unsigned char texdata[400];
        for(int i = 0; i < data.size(); i++){
            for(int j = 0; j < data[i].size(); j++){
                texdata[i * 40 + j * 4] = ((bool)data[i][j]) ? 255 : 0;
                texdata[i * 40 + j * 4 + 1] = ((bool)data[i][j]) ? 255 : 0;
                texdata[i * 40 + j * 4 + 2] = ((bool)data[i][j]) ? 255 : 0;
                texdata[i * 40 + j * 4 + 3] = 255;
            }
        }
        glBindTexture(GL_TEXTURE_2D, texture);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 10, 10, 0, GL_RGBA, GL_UNSIGNED_BYTE, texdata);
    }

    void load_texture_to_atlas(unsigned char* atlas, int row_offset, int row_stride, int column_offset, bool flip_vertical = false, bool flip_horiz = false, int min = 0, int max = 10){
        unsigned char *temp_texture = nullptr;
        int own_stride = 40;
        if(texture == nullptr){
            temp_texture = new unsigned char[4 * 10 * 10];
        }
        for(int i = min; i < data.size() && i < max; i++){
            int index = i - min;
            if(flip_vertical){
                index = (data.size() - i - 1) - min;
            }
            for(int j = min; j < data[i].size() && j < max; j++){
                int index2 = j - min;
                if(flip_horiz){
                    index2 = (data[i].size() - j - 1) - min;
                }
                if(texture == nullptr){
                    atlas[((index) + row_offset) * row_stride + (index2 + column_offset) * 4] = ((bool)data[i][j]) ? 255 : 0;
                    atlas[((index) + row_offset) * row_stride + (index2 + column_offset) * 4 + 1] = ((bool)data[i][j]) ? 255 : 0;
                    atlas[((index) + row_offset) * row_stride + (index2 + column_offset) * 4 + 2] = ((bool)data[i][j]) ? 255 : 0;
                    atlas[((index) + row_offset) * row_stride + (index2 + column_offset) * 4 + 3] = 255;
                    temp_texture[index * own_stride + index2 * 4] = ((bool)data[i][j]) ? 255 : 0;
                    temp_texture[index * own_stride + index2 * 4 + 1] = ((bool)data[i][j]) ? 255 : 0;
                    temp_texture[index * own_stride + index2 * 4 + 2] = ((bool)data[i][j]) ? 255 : 0;
                    temp_texture[index * own_stride + index2 * 4 + 3] = 255;
                }
                else {
                    atlas[((index) + row_offset) * row_stride + (index2 + column_offset) * 4] = texture[index * own_stride + index2 * 4];
                    atlas[((index) + row_offset) * row_stride + (index2 + column_offset) * 4 + 1] = texture[index * own_stride + index2 * 4 + 1];
                    atlas[((index) + row_offset) * row_stride + (index2 + column_offset) * 4 + 2] = texture[index * own_stride + index2 * 4 + 2];
                    atlas[((index) + row_offset) * row_stride + (index2 + column_offset) * 4 + 3] = texture[index * own_stride + index2 * 4 + 3];
                }
            }
        }
        if(texture == nullptr){
            texture = temp_texture;
        }
    }

    void set_monster(int transformed_x, int transformed_y){
        int y, x, own_stride = 4 * 10;
        if(texture == nullptr){
            return;
        }

        texture[y * own_stride + x * 4 + 1] = 0;
        texture[y * own_stride + x * 4 + 2] = 0; 
    }

    std::array<std::bitset<10>, 10> get_transformed(){
        std::array<std::bitset<10>, 10> transformed_data;
        for(int row = 0; row < data.size(); row++){
            for(int col = 0; col < data.size(); col++){
                int outrow, outcol;
                switch((vflipped << 3 ) | (hflipped << 2) | (orientation & 0x3)){
                case 0b1110:
                case 0b0000:
                    outrow = row;
                    outcol = col;
                    break;
                case 0b1111:
                case 0b0001:
                    outrow = 9 - col;
                    outcol = row;
                    break;
                case 0b1100:
                case 0b0010:
                    outrow = 9 - row;
                    outcol = 9 - col;
                    break;
                case 0b1101:
                case 0b0011:
                    outrow = col;
                    outcol = 9 - row;
                    break;
                case 0b0100:
                    outrow = row;
                    outcol = 9 - col;
                    break;
                case 0b0101:
                    outrow = 9 - col;
                    outcol = 9 - row;
                    break;
                case 0b0110:
                    outrow = 9 - row;
                    outcol = col;
                    break;
                case 0b0111:
                    outrow = col;
                    outcol = row;
                    break;
                case 0b1000:
                    outrow = 9 - row;
                    outcol = col;
                    break;
                case 0b1001:
                    outrow = col;
                    outcol = row;
                    break;
                case 0b1010:
                    outrow = row;
                    outcol = 9 - col;
                    break;
                case 0b1011:
                    outrow = 9 - col;
                    outcol = 9 - row;
                    break;
                
                }
                transformed_data[outrow][outcol] = data[row][col];
            }
        }
        return transformed_data;
    }

    void load_texture_to_atlas_transformed(unsigned char* atlas, int row_offset, int row_stride, int column_offset, bool flip_vertical = false, bool flip_horiz = false, int min = 0, int max = 10){
        
        std::array<std::bitset<10>, 10> transformed_data = get_transformed();
        
        for(int i = min; i < transformed_data.size() && i < max; i++){
            int index = i - min;
            if(flip_vertical){
                index = (transformed_data.size() - i - 1) - min;
            }
            for(int j = min; j < transformed_data[i].size() && j < max; j++){
                int index2 = j - min;
                if(flip_horiz){
                    index2 = (transformed_data[i].size() - j - 1) - min;
                }
                atlas[((index) + row_offset) * row_stride + (index2 + column_offset) * 4] = ((bool)transformed_data[i][j]) ? 255 : 0;
                atlas[((index) + row_offset) * row_stride + (index2 + column_offset) * 4 + 1] = ((bool)transformed_data[i][j]) ? 255 : 0;
                atlas[((index) + row_offset) * row_stride + (index2 + column_offset) * 4 + 2] = ((bool)transformed_data[i][j]) ? 255 : 0;
                atlas[((index) + row_offset) * row_stride + (index2 + column_offset) * 4 + 3] = 255;
            }
        }
    }

    template <unsigned X, unsigned Y>
    void load_bits_to_atlas_transformed(std::array<std::bitset<X>, Y>& atlas, int row_offset, int column_offset, bool flip_vertical = false, bool flip_horiz = false, int min = 0, int max = 10){
        
        std::array<std::bitset<10>, 10> transformed_data = get_transformed();
        
        for(int i = min; i < transformed_data.size() && i < max; i++){
            int index = i - min;
            if(flip_vertical){
                index = (transformed_data.size() - i - 1) - min;
            }
            for(int j = min; j < transformed_data[i].size() && j < max; j++){
                int index2 = j - min;
                if(flip_horiz){
                    index2 = (transformed_data[i].size() - j - 1) - min;
                }
                atlas[index + row_offset][(X - 1) - (index2 + column_offset)] = transformed_data[i][j];
            }
        }
    }

    void load_vertices(rjs::mesh& mesh, int x, int y){
        float vertices[40];
        uint indices[36] = {
            0, 1, 2,
            2, 3, 0,
            
            0, 3, 4,
            4, 3, 7,

            0, 4, 1,
            1, 4, 5,

            1, 5, 2,
            2, 5, 6,

            2, 6, 3,
            3, 6, 7,

            4, 7, 5,
            5, 7, 6
        };
        float offsets[24] = {
            -0.5, -0.5, -0.0625, 
             0.5, -0.5, -0.0625, 
             0.5,  0.5, -0.0625, 
            -0.5,  0.5, -0.0625, 
            -0.5, -0.5,  0.0625, 
             0.5, -0.5,  0.0625, 
             0.5,  0.5,  0.0625, 
            -0.5,  0.5,  0.0625
        };
        for(int i = 0; i < tex_coords.size(); i++){
            vertices[i * 5] = offsets[i * 3];
            vertices[i * 5 + 1] = offsets[i * 3 + 1];
            vertices[i * 5 + 2] = offsets[i * 3 + 2];
            vertices[i * 5 + 3] = tex_coords[(i * 2) % 8];
            vertices[i * 5 + 4] = tex_coords[(i * 2 + 1) % 8];
        }

        mesh.buffer(vertices, sizeof(float), 40, indices, sizeof(uint), 36, GL_STATIC_DRAW);
        std::vector<std::pair<unsigned, size_t>> layout = { {3, sizeof(float)}, {2, sizeof(float)} };
        mesh.setup_vertex_pointer(layout, GL_FLOAT, 5 * sizeof(float));
    }

    std::pair<std::array<float, 40>, std::array<int, 36>> get_vertex_data(int x, int y){
        std::array<float, 40> vertices;
        std::array<int, 36> indices = {
            0, 1, 2,
            2, 3, 0,
            
            0, 3, 4,
            4, 3, 7,

            0, 4, 1,
            1, 4, 5,

            1, 5, 2,
            2, 5, 6,

            2, 6, 3,
            3, 6, 7,

            4, 7, 5,
            5, 7, 6
        };
        std::array<rjs::vertex, 8> offsets = {
            rjs::vertex{-0.5, -0.5, -0.125}, {0.5, -0.5, -0.125}, {0.5, 0.5, -0.125}, {-0.5, 0.5, -0.125}, 
            {-0.5, -0.5, 0.125}, {0.5, -0.5, 0.125}, {0.5, 0.5, 0.125}, {-0.5, 0.5, 0.125}
        };
        std::array<float, 8> tex_coords = {
            0.0f, 1.0f,
            1.0f, 1.0f,
            1.0f, 0.0f,
            0.0f, 0.0f
        };
        rjs::vertex base{(float) x, (float) y, 0.0f};
        for(int i = 0; i < offsets.size(); i++){
            rjs::vertex curr = base + offsets[i];
            vertices[i * 5] = curr.x;
            vertices[i * 5 + 1] = curr.y;
            vertices[i * 5 + 2] = curr.z;
            vertices[i * 5 + 3] = tex_coords[(i * 2) % 8];
            vertices[i * 5 + 4] = tex_coords[(i * 2 + 1) % 8];
        }
        return std::make_pair(vertices, indices);
    }

    void translate_to(int x, int y){
        target_x = x;
        target_y = y;
    }

    void translate(int dx, int dy){
        target_x += dx;
        target_y += dy;
    }

    void update_translate(double deltaTime){
        if(glm::abs(target_x - current_x) < 0.1 && glm::abs(target_y - current_y) < 0.1){
            current_x = target_x;
            current_y = target_y;
        } else if(target_x < current_x){
            current_x -= 5 * deltaTime;
            if(glm::abs(target_x - current_x) < 0.1){
                current_x = target_x;
            }
        } else if(target_x > current_x){
            current_x += 5 * deltaTime;
            if(glm::abs(target_x - current_x) < 0.1){
                current_x = target_x;
            }
        } else if(target_y < current_y){
            current_y -= 5 * deltaTime;
            if(glm::abs(target_y - current_y) < 0.1){
                current_y = target_y;
            }
        } else if(target_y > current_y){
            current_y += 5 * deltaTime;
            if(glm::abs(target_y - current_y) < 0.1){
                current_y = target_y;
            }
        }

        if(glm::abs(target_x - current_x) < 0.1 && glm::abs(target_y - current_y) < 0.1){
            current_x = target_x;
            current_y = target_y;
        }
    }

    bool translating(){
        return target_x != current_x || target_y != current_y;
    }

    float curr_x(){
        return current_x;
    }

    float curr_y(){
        return current_y;
    }

    void rotate(direction d){
        if(hflipped != vflipped){
            d = (d == direction::CCW) ? direction::CW : direction::CCW;
        }
        switch(d){
        case direction::CW:
            orientation = (orientation + 1) % 4;
            target_rotate = target_rotate - 90;
            if(target_rotate < -360){
                target_rotate += 360;
            }
            if(target_rotate > current_rotate){
                current_rotate += 360;
            }
            break;
        case:: direction::CCW:
            orientation = (orientation + 3) % 4;
            target_rotate = target_rotate + 90;
            if(target_rotate > 360){
                target_rotate -= 360;
            }
            if(target_rotate < current_rotate){
                current_rotate -= 360;
            }
            break;
        }
    }

    void update_rotate(double deltaTime){
        if(glm::abs(target_rotate - current_rotate) < 1){
            current_rotate = target_rotate;
        } else if(target_rotate < current_rotate){
            current_rotate -= deltaTime * 90;
        } else if(target_rotate > current_rotate){
            current_rotate += deltaTime * 90;
        }
    }

    float get_rotate(){
        return current_rotate;
    }

    bool rotating(){
        return glm::abs(target_rotate - current_rotate) > 0.1;
    }

    void hflip(){
        if(target_hflip >= 360){
            if(current_hflip == target_hflip)
                current_hflip -= 360;
            target_hflip -= 360;
        }
        target_hflip = target_hflip + 180;
        hflipped = !hflipped;
        //orientation = (orientation + 2) % 4;
    }

    void update_hflip(double deltaTime){
        if(current_hflip < target_hflip){
            current_hflip += 180 * deltaTime;
        }
        if(current_hflip >= target_hflip) {
            current_hflip = target_hflip;
        }
    }

    float get_hflip(){
        return current_hflip;
    }

    void vflip(){
        if(target_vflip >= 360){
            if(current_vflip == target_vflip)
                current_vflip -= 360;
            target_vflip -= 360;
        }
        target_vflip = target_vflip + 180;
        vflipped = !vflipped;
        //orientation = (orientation + 2) % 4;
    }

    void update_vflip(double deltaTime){
        if(current_vflip < target_vflip){
            current_vflip += 180 * deltaTime;
        }
        if(current_vflip >= target_vflip) {
            current_vflip = target_vflip;
        }
    }

    float get_vflip(){
        return current_vflip;
    }

    bool hflipping(){
        return target_hflip != current_hflip;
    }

    bool vflipping(){
        return target_vflip != current_vflip;
    }
};
#endif