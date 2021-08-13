#version 330 core 

out vec4 fargColor;
in vec2 textureCor;

uniform sampler2D texture_diffuse1;

void main(){
    fargColor = texture(texture_diffuse1, textureCor);
}