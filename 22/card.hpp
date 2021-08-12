#ifndef CARD_HPP
#define CARD_HPP

#include <glad/glad.h>
#include <opengl/texture.hpp>
#include <opengl/character.hpp>
#include <opengl/mesh.hpp>
#include <stb_image.h>
#include <iostream>
#include <freetype2/ft2build.h>
#include FT_FREETYPE_H

class card {
private:
    static unsigned card_faces;
    static FT_Library ft;
    static FT_Face font;
    static unsigned references;
    unsigned number_tex = 0, value;
    rjs::mesh mesh;

    unsigned load_texture(unsigned texture, const char* filename){
        int w, h, ch;
        unsigned char* data = stbi_load(filename, &w, &h, &ch, 4);
        if(data == nullptr){
            std::cerr << "Failed to load " << filename << std::endl;
            exit(EXIT_FAILURE);
        }
        buffer_texture(texture, w, h, data);
        stbi_image_free(data);
        return texture;
    }

    void buffer_texture(unsigned texture, int w, int h, unsigned char* data){
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void set_face_coords(float* vertices, float bottom, float left, float top, float right){
        vertices[23] = left;
        vertices[24] = bottom;

        vertices[28] = right;
        vertices[29] = bottom;

        vertices[33] = right;
        vertices[34] = top;

        vertices[38] = left;
        vertices[39] = top;
    }

public:
    card(unsigned value, rjs::mesh mesh)
        : value(value)
        , mesh(std::move(mesh)) 
    {
        std::vector<unsigned> generated;
        if(card_faces == 0){
            generated = rjs::texture::generate(2);
            card_faces = load_texture(generated[1], "resources/card_atlas.png");
        } else {
            generated = rjs::texture::generate(1);
        }
        if(ft == nullptr && FT_Init_FreeType(&ft)){
            std::cerr << "Error: Could not initialize freetype library" << std::endl;
            exit(EXIT_FAILURE);
        }
        if(font == nullptr && FT_New_Face(ft, "resources/georgia.ttf", 0, &font)){
            std::cerr << "Error: Could not load font " << "resources/georgia.ttf" << std::endl;
            exit(EXIT_FAILURE);
        }
        
        number_tex = generated[0];
//TODO: create text
        float vertices[40] = {
            -1.125f, -1.71f, -0.005f, 0.0f, 0.5f,
             1.125f, -1.71f, -0.005f, 0.25f, 0.5f,
             1.125f,  1.71f, -0.005f, 0.25f, 1.0f,
            -1.125f,  1.71f, -0.005f, 0.0f, 1.0f,
            -1.125f, -1.71f,  0.005f, 0.0f, 0.0f,
             1.125f, -1.71f,  0.005f, 0.0f, 0.0f,
             1.125f,  1.71f,  0.005f, 0.0f, 0.0f,
            -1.125f,  1.71f,  0.005f, 0.0f, 0.0f,
        };
        unsigned* indices = rjs::mesh::rect_prism_indices();
        switch(value % 4){
        case 0:
            set_face_coords(vertices, 0.5f, 0.25f, 1.0f, 0.5f);
            break;
        case 1:
            set_face_coords(vertices, 0.5f, 0.5f, 1.0f, 0.75f);
            break;
        case 2:
            set_face_coords(vertices, 0.5f, 0.75f, 1.0f, 1.0f);
            break;
        case 3:
            set_face_coords(vertices, 0.0f, 0.0f, 0.5f, 0.25f);
            break;
        }
        this->mesh.buffer(vertices, sizeof(float), 40, indices, sizeof(unsigned), 36, GL_STATIC_DRAW);
        this->mesh.setup_vertex_pointer({{3, sizeof(float)}, {2, sizeof(float)}}, GL_FLOAT, sizeof(float) * 5);
        delete[] indices;
        references++;
    }

    card(const card& other){
        
    }

    ~card(){
        references--;
        if(references == 0){

        }
    }

    void draw(){
        glBindTexture(GL_TEXTURE_2D, card_faces);
        mesh.draw();
        glBindTexture(GL_TEXTURE_2D, 0);
    }

};

unsigned card::card_faces = 0;
FT_Library card::ft = nullptr;
FT_Face card::font = nullptr;
unsigned card::references = 0;

#endif