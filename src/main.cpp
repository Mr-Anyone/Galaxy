#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "camera.h"
#include "shader.h"
#include "mesh.h"
#include "model.h"

constexpr int width_g {800}; 
constexpr int height_g {600}; 
constexpr int rockCount {10000};
Camera camear_g {glm::vec3(300.0f, 5.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)};
glm::mat4 projection_g {glm::perspective(glm::radians(45.0f), static_cast <float> (width_g) / height_g, 0.1f, 100000.0f)};

void init()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    stbi_set_flip_vertically_on_load(true);
}

void processInput(GLFWwindow* window)
{  
    if(glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true); 

    constexpr float cameraSpeed {3.0f};
    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camear_g.moveFront(cameraSpeed);
    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camear_g.moveFront(-cameraSpeed);
    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camear_g.moveSide(-cameraSpeed);
    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camear_g.moveSide(cameraSpeed);
}

void clearBuffer()
{
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f); 
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void draw(const Shader& shader, const Mesh& mesh )
{
    shader.use(); 
    
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection_g));
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(glm::mat4 (1.0f)));
    glUniformMatrix4fv(glGetUniformLocation(shader.ID,  "view"), 1, GL_FALSE, glm::value_ptr(camear_g.getView()));
    mesh.draw();
}
void drawModel(const Shader& shader, const Model& model)
{
    shader.use(); 
    
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection_g));
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(glm::mat4 (1.0f)));
    glUniformMatrix4fv(glGetUniformLocation(shader.ID,  "view"), 1, GL_FALSE, glm::value_ptr(camear_g.getView()));
    model.draw(shader);
}

void mouseCallback(GLFWwindow* window, double xPos, double yPos)
{
    static double pX, pY; 
    static bool first {true};
    if(first)
    {
        pX = xPos; 
        pY = yPos; 
        first = false;
        return;
    }

    camear_g.rotate(xPos - pX, pY - yPos);
    pX = xPos; 
    pY = yPos;
}

void drawPlanet(const Shader& shader, const Model& model)
{
    shader.use(); 
    glm::mat4 scale (1.0f); 
    scale = glm::scale(scale, glm::vec3(50.0f, 50.0f, 50.0f));

    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection_g));
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(scale));
    glUniformMatrix4fv(glGetUniformLocation(shader.ID,  "view"), 1, GL_FALSE, glm::value_ptr(camear_g.getView()));
    model.draw(shader);
}

void drawRocks(const Shader& shader, const Model& model, const glm::mat4* positions)
{
    shader.use();
    
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection_g));
    glUniformMatrix4fv(glGetUniformLocation(shader.ID,  "view"), 1, GL_FALSE, glm::value_ptr(camear_g.getView()));
    // Draw 
    for(int i = 0; i<model.m_meshes.size(); ++i)
    {
        glBindVertexArray(model.m_meshes[i].VAO); 
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model.m_meshes[i].EBO); 
        glDrawElementsInstanced(GL_TRIANGLES, model.m_meshes[i].m_indicesSize, GL_UNSIGNED_INT, 0, rockCount);
    }

}

void makeLocations(glm::mat4* array, int size, Model& rock)
{
    constexpr float radius {300.f};
    constexpr int offset {40};
    
    for (int i = 0; i<size; ++i)
    {
        glm::mat4 model (1.0f); 
        auto degree {rand()};
        
        int xOffset {rand() % offset  - offset/2};
        int yOffset {rand() % 40 - 5} ;
        int zOffset {rand() % offset - offset/2};

        float x = cos(degree) * radius + xOffset ;
        float y = yOffset;
        float z = sin(degree) * radius + zOffset;
        
        model = glm::translate(model, glm::vec3(x, y, z));
        

        array[i] = model;
    }
    
    // Create instance class
    unsigned int buffer;
    glGenBuffers(1, &buffer); 
    glBindBuffer(GL_ARRAY_BUFFER, buffer); 
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * rockCount, array, GL_STATIC_DRAW); 
    for(int i =0; i<rock.m_meshes.size(); ++i)
    {
        glBindVertexArray(rock.m_meshes[i].VAO);

        // Matrix 4 takes up for space
        std::size_t vec4Size = sizeof(glm::vec4);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)0);
        glEnableVertexAttribArray(3); 

        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(1 * vec4Size));
        glEnableVertexAttribArray(4); 
        
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(2 * vec4Size));
        glEnableVertexAttribArray(5); 
        
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(3 * vec4Size));
        glEnableVertexAttribArray(6); 
        
        glVertexAttribDivisor(3, 1);
        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);
        glVertexAttribDivisor(6, 1);
    }
}

int main()
{ 
    std::cout << "Build Success"<< std::endl;
    init();
    GLFWwindow* window {glfwCreateWindow(width_g, height_g, "OpenGL Window", nullptr, nullptr)};
    if(window == nullptr)
    {
        std::cerr << "Cannot Make Window Context" << std::endl; 
        std::exit(-1);
    }
    glfwMakeContextCurrent(window); 
    
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        std::exit(-1);
    }

    glViewport(0, 0, width_g, height_g);
    Shader shader {"./../res/shader/vertexShader.glsl", "./../res/shader/fragmentShader.glsl"};
    Shader rockShader {"./../res/shader/instanceVertexShader.glsl", "./../res/shader/instanceFragmentShader.glsl"}; 

    float vertices  [] {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.0f, 0.5f, 0.0f,
    };
    unsigned int indices []  {
        0, 1, 2
    }; 

    Mesh triangle {vertices, indices, 9, 3};
    triangle.loadData();

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouseCallback);

    Model rock {"./../res/model/rock/rock.obj"};
    Model planet {"./../res/model/planet/planet.obj"};

    glEnable(GL_DEPTH_TEST);  
    glm::mat4* locations { new glm::mat4[rockCount]};
    makeLocations(locations, rockCount, rock);

    while(!glfwWindowShouldClose(window))
    {
        // This on only clears color buffer
        clearBuffer();
        processInput(window);
        
        drawPlanet(shader, planet);
        drawRocks(rockShader, rock, locations);

        glfwPollEvents(); 
        glfwSwapBuffers(window);
    }
    return 0;
}