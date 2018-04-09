#version 330 core

in vec2 UV;
out vec3 color;

uniform sampler2D game_texture;
uniform sampler2D fow_texture;

void main() {
    vec3 fow_value = texture(fow_texture, UV).rgb;
    vec3 color_value = texture(game_texture, UV).rgb;

    color = color_value * fow_value;
}