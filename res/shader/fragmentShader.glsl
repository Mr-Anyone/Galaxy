#version 330 core 
out vec4 fragColor;

in vec2 textureCor;

uniform sampler2D texture_specular1;

void main(){
    fragColor = texture(texture_specular1, textureCor);
}