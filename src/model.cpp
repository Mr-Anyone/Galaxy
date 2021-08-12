#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>
#include <array> 
#include <string>
#include <stb_image.h>

#include "model.h"


void Model::draw(const Shader& shader) const
{
    this->loadTextures(shader);
    for(auto& ele : m_meshes)
        ele.draw(shader);
}


void Model::loadModel(const char* modelPath)
{
    Assimp::Importer importer; 
    const aiScene * scene { importer.ReadFile(modelPath, aiProcess_Triangulate | aiProcess_FlipUVs)}; 
    
    if(!scene | scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cout << "Cannot Load Model With Error: " << importer.GetErrorString() << std::endl; 
        std::exit(-1);
    }
   
    processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode *node, const aiScene*scene)
{
    for(unsigned int i=0; i<node->mNumMeshes; ++i)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]]; 
        m_meshes.push_back(processMesh(mesh, scene)); // add move operator to change it into std::move (a)
    }

    for(unsigned int i=0; i<node->mNumChildren; ++i)
    {
        processNode(node->mChildren[i], scene);
    }
}

static unsigned int TextureFromFile(const char* filename)
{
    std::string path {"./../res/model/backpack/"}; 
    path += filename; 

    unsigned int textureID;
    glGenTextures(1, &textureID); 
    
    int width, height, channels;
    unsigned char* imageData {stbi_load(path.c_str(), &width, &height, &channels, 0)}; 
    if(imageData)
    {
        GLenum format;
        if (channels == 1)
            format = GL_RED;
        else if (channels == 3)
            format = GL_RGB;
        else if (channels == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, imageData);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(imageData);

    }
    else
    {
        std::cout << path << std::endl;
        std::cout << "Cannot Load Texture" << std::endl;
    }

    return textureID;
}

static std::vector<Texture> loadMaterialTexture(aiMaterial *mat, aiTextureType type, const char* typeName)
{
    std::vector<Texture> textures;
    static std::vector<Texture> loadedTextures; 

    for(unsigned int i = 0; i< mat->GetTextureCount(type); ++i)
    {
        aiString* str {new aiString};
        mat->GetTexture(type, i, str);
        bool skip {false};
        
        for(int j =0; j< loadedTextures.size(); ++j)
        {
            if(strcmp(loadedTextures[j].path, str->C_Str()) == 0)
            {
                textures.push_back(loadedTextures[j]);
                skip = true;
                break;
            }
        }

        if(!skip)
        {
            Texture texture;
            texture.id = TextureFromFile(str->C_Str());
            texture.type = typeName;
            texture.path = str->C_Str();
            textures.push_back(texture);    
            loadedTextures.push_back(texture);
        }
    }
    
    return textures;
}

ModelMesh&&  Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
    const std::size_t verticesSize = mesh->mNumVertices * 8;
    const std::size_t indicesSize = mesh->mNumFaces * 3;
    const std::size_t textureSize = 0;

    float* vertices {new float [verticesSize]}; // 8 as the stride
    unsigned int* indices {new unsigned int [indicesSize]}; // Each face has two indices
    Texture* textures {new Texture [textureSize]};
    
    int count {0}; // Keep track of the count
    for(unsigned int i = 0; i<mesh->mNumVertices; ++i)
    {
        // Position
        vertices[count] = mesh->mVertices[i].x; 
        vertices[count + 1] = mesh->mVertices[i].y; 
        vertices[count+ 2] = mesh->mVertices[i].z; 

        // Normals
        vertices[count + 3] = mesh->mNormals[i].x; 
        vertices[count + 4] = mesh->mNormals[i].y; 
        vertices[count+ 5] = mesh->mNormals[i].z; 

        // Texture Coordinates
        if(mesh->mTextureCoords[0]) // have texures coords ?
        {
            vertices[count + 6] = mesh->mTextureCoords[0][i].x;
            vertices[count + 7] = mesh->mTextureCoords[0][i].y;
        }
        else
        {
            vertices[count + 6] = 0;
            vertices[count + 7] = 0;
        }

        count += 8; 
    }
    count = 0;

    for(unsigned int i = 0; i<mesh->mNumFaces; ++i)
    {
        aiFace face = mesh->mFaces[i]; 
        for(unsigned int j = 0; j<face.mNumIndices; ++j)
        {
            indices[count] = face.mIndices[j];
            ++count;
        }
    }
    count = 0;
    
    if(mesh->mMaterialIndex >=0)
    {
        aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
        std::vector<Texture> diffuseMap  {loadMaterialTexture(material, aiTextureType_DIFFUSE, "texture_diffuse")};
        m_textures.insert(m_textures.end(), diffuseMap.begin(), diffuseMap.end());

        std::vector<Texture> specularMap { loadMaterialTexture(material, aiTextureType_SPECULAR, "texture_specular")};
        m_textures.insert(m_textures.end(), specularMap.begin(), specularMap.end());
    }
    
    ModelMesh modelMesh {vertices, indices, verticesSize, indicesSize};
    modelMesh.loadData();

    // free memory
    delete [] vertices; 
    delete [] indices; 
    delete [] textures; 
    return std::move(modelMesh);
}

void Model::loadTextures(const Shader& shader) const {
    int specularNum {1}; 
    int diffuseNum {1}; 

    for(int i=0; i<m_textures.size(); ++i)
    {
        glActiveTexture(GL_TEXTURE0 + i);

        std::string name;
        if(strcmp(m_textures[i].type, "texture_diffuse")== 0)
        {
            name = "texture_diffuse" + std::to_string(diffuseNum++);
        }

        if(strcmp(m_textures[i].type, "texture_specular") == 0)
        {
            name = "textures_specular" + std::to_string(specularNum++);
        }
        
        glUniform1i(glGetUniformLocation(shader.ID, name.c_str()), i);
        glBindTexture(GL_TEXTURE_2D, m_textures[i].id);
    }
}