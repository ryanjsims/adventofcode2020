#version 330 core
out vec4 FragColor;
in vec2 texCoord;

uniform sampler2D tile_texture;
uniform vec3 selected;

void main(){
    FragColor = vec4(mix(texture(tile_texture, texCoord).rgb, vec3(selected), 0.10), 1.0);
}