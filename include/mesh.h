#ifndef MESH_H 
#define MESH_H 

#include <cstddef>
#include "shader.h"

// This class could use smart pointer but I have no idea how to use smart pointer properly
class Mesh{
protected:
    float* m_vertices; 
    unsigned int* m_indices;
    std::size_t m_verticesSize; 
    std::size_t m_indicesSize;

    unsigned int VAO, VBO, EBO;    
public: 
    Mesh()=delete;

    Mesh(float* vertices, unsigned int* indices, std::size_t verticeSize, std::size_t indicesSize);
    ~Mesh();

    // Diable copying
    Mesh(const Mesh& mesh) =delete;
    Mesh& operator=(const Mesh& mesh) = delete;

    // Move consturctor
    Mesh(Mesh&& mesh); 

    virtual void loadData();
    void draw() const;
    void draw(const Shader& shader) const;

};

struct Texture{
    unsigned int id; 
    const char* path;
    const char* type;
}; 

class ModelMesh: public Mesh {
public: 
    ModelMesh(float* vertices, unsigned int* indices, std::size_t verticeSize, std::size_t indicesSize):
        Mesh{vertices, indices, verticeSize, indicesSize}
    {
    }
    
    
    // Disable Copying
    ModelMesh(const ModelMesh& mesh) = delete; 
    ModelMesh& operator=(const ModelMesh& mesh) = delete;

    // Move operator 
    ModelMesh(ModelMesh&& mesh);
    

    ~ModelMesh();

    virtual void loadData();
};

#endif