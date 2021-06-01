#ifndef MESH_HPP
#define MESH_HPP

#include <glad/glad.h>

#include <vector>

//#error "mesh.hpp not implemented"

namespace rjs {
    class mesh {
    public:
        static std::vector<mesh> generate(unsigned n){
            unsigned *VAOs = new unsigned[n];
            unsigned *VBOs = new unsigned[n];
            unsigned *EBOs = new unsigned[n];
            glGenVertexArrays(n, VAOs);
            glGenBuffers(n, VBOs);
            glGenBuffers(n, EBOs);
            std::vector<mesh> generated;
            for(unsigned i = 0; i < n; i++){
                generated.push_back(mesh(VAOs[i], VBOs[i], EBOs[i]));
            }
            delete[] VAOs;
            delete[] VBOs;
            delete[] EBOs;
            return generated;
        }

        mesh(){
            
        }

        mesh(unsigned VAO, unsigned VBO, unsigned EBO){
            this->VAO = VAO;
            this->VBO = VBO;
            this->EBO = EBO;
        }

        void discard(){
            glDeleteVertexArrays(1, &VAO);
            glDeleteBuffers(1, &VBO);
            glDeleteBuffers(1, &EBO);
            VAO = 0;
            VBO = 0;
            EBO = 0;
        }

        void bind() const {
            glBindVertexArray(VAO);
        }

        void buffer(const void* vertices, size_t vertex_bytes, size_t vertex_count, const void* indices, size_t index_bytes, size_t index_count, GLenum usage){
            bind();
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, vertex_bytes * vertex_count, vertices, usage);
            this->vertices = vertex_count;
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_bytes * index_count, indices, usage);
            this->elements = index_count;
        }

        /**
         * layout - vector of pairs of (element count, element size)
         * type - type of the data being arranged
         * stride - overall stride of the vertex pointer
         */
        void setup_vertex_pointer(std::vector<std::pair<unsigned, size_t>> layout, GLenum type, size_t stride){
            bind();
            size_t offset = 0;
            for(unsigned i = 0; i < layout.size(); i++){
                glVertexAttribPointer(i, layout[i].first, type, GL_FALSE, stride, (void*)offset);
                glEnableVertexAttribArray(i);
                offset += layout[i].first * layout[i].second;
            }
        }

        void draw() const {
            this->bind();
            glDrawElements(GL_TRIANGLES, this->element_count(), GL_UNSIGNED_INT, nullptr);
        }

        int element_count() const {
            return elements;
        }

        int vertex_count() const {
            return vertices;
        }

    private:
        unsigned VAO = 0, VBO = 0, EBO = 0;
        int elements = 0, vertices = 0;
    };
}
#endif