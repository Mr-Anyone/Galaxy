#ifndef MODEL_H
#define MODEL_H 

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>
#include <glm/glm.hpp>

#include "shader.h"
#include "mesh.h"

class Model{
    std::vector<ModelMesh> m_meshes {};
    std::vector<Texture> m_textures {};
    int count {0}; 

    void loadTextures(const Shader& shader) const;
    std::string m_directory;
public: 
    Model(const char* modelPath ) 
    {
        m_directory = modelPath;
        m_directory = m_directory.substr(0, m_directory.find_last_of('/') + 1 );
        this->loadModel(modelPath);
    }


    void loadModel(const char* modelPath);
    void processNode(aiNode *node, const aiScene*scene); 
    ModelMesh processMesh(aiMesh *mesh, const aiScene *scene);
    void draw(const Shader& shader) const;
    friend void makeLocations(glm::mat4* array, int size, Model& rock); // Think of a better way to access private
    friend void drawRocks(const Shader& shader, const Model& model, const glm::mat4* positions);
}; 

#endif
