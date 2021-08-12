#version 330 core 
layout (location=0) in vec3 aPos;
layout (location=1) in vec3 aNormal;
layout (location=2) in vec2 aTextureCor;

out vec2 textureCor; 

uniform mat4 projection; 
uniform mat4 model; 
uniform mat4 view;

void main(){
    gl_Position = projection * model * view * vec4(aPos, 1.0f);
    textureCor = aTextureCor;
}