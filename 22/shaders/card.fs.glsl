#version 330 core
out vec4 FragColor;
in vec2 texCoord;

uniform sampler2D card_atlas;

void main(){
    FragColor = texture(card_atlas, texCoord);
}