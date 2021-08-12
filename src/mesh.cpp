#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "mesh.h"
#include "shader.h"

void Mesh::loadData()
{
    glGenVertexArrays(1, &VAO); 
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO); 
    glBindBuffer(GL_ARRAY_BUFFER, VAO); 
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * m_verticesSize, m_vertices, GL_STATIC_DRAW); 

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0); 

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); 
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * m_indicesSize, m_indices, GL_STATIC_DRAW); 

    glBindVertexArray(0);
}

void Mesh::draw() const
{
    glBindVertexArray(VAO); 
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glDrawElements(GL_TRIANGLES, m_indicesSize, GL_UNSIGNED_INT, 0); 
}

void Mesh::draw(const Shader& shader) const 
{
    shader.use(); 
    glBindVertexArray(VAO); 
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glDrawElements(GL_TRIANGLES, m_indicesSize, GL_UNSIGNED_INT, 0); 
}

void ModelMesh::loadData(){
    glGenVertexArrays(1, &VAO); 
    glGenBuffers(1, &EBO);
    glGenBuffers(1, &VBO); 

    glBindVertexArray(VAO); 
    glBindBuffer(GL_ARRAY_BUFFER, VBO); 
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * m_verticesSize, m_vertices, GL_STATIC_DRAW); 

    // Format position (x, y, z), normal (x, y, z), texture (x, y)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0); 

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1); 

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(2); 

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); 
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * m_indicesSize, m_indices, GL_STATIC_DRAW); 

    glBindVertexArray(0);
}

 Mesh::Mesh(float* vertices, unsigned int* indices, std::size_t verticeSize, std::size_t indicesSize):
        m_verticesSize{verticeSize}, m_indicesSize {indicesSize}
    {
        // Copying data 
        m_vertices = new float [verticeSize];
        for(int i = 0; i<verticeSize; ++i)
        {
            m_vertices[i] = vertices[i]; 
        }
        
        m_indices = new unsigned int [indicesSize]; 
        for(int i =0; i<indicesSize; ++i)
        {
            m_indices[i] = indices[i];
        }

    }

Mesh::Mesh(Mesh&& mesh)
{
    m_vertices = mesh.m_vertices; 
    m_indices = mesh.m_indices;
    m_verticesSize = mesh.m_verticesSize;
    m_indicesSize = mesh.m_indicesSize;
    VAO = mesh.VAO; 
    VBO = mesh.VBO; 
    EBO = mesh.EBO;

    mesh.m_vertices = nullptr;
    mesh.m_indices = nullptr;
}


Mesh::~Mesh()
{
    if(m_vertices != nullptr)
    {
        delete [] m_vertices; 
        m_vertices = nullptr;
    }

    if(m_indices != nullptr)
    {
        delete [] m_indices; 
        m_indices = nullptr;
    }
}

ModelMesh::ModelMesh(ModelMesh && mesh ):
    Mesh {std::move(mesh)}
{
}

ModelMesh::~ModelMesh()
{

}