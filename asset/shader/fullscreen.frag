#version 330 core

in vec2 UV;
out vec3 color;

uniform sampler2D game_texture;

void main() {
    color = texture(game_texture, UV).rgb;
}