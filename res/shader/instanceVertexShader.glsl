#version 330 core 
layout (location=0) in vec3 aPos;
layout (location=1) in vec3 aNormal;
layout (location=2) in vec2 aTextureCor;
layout (location=3) in mat4 model;
// 3, 4, 5, 6 reserved by model (mat4)

uniform mat4 projection;
uniform mat4 view;

out vec2 textureCor;
void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0f);   
    textureCor = aTextureCor;
}