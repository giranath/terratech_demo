#version 330 core

in vec2 UV;
out vec4 color;

uniform sampler2D game_texture;
uniform sampler2D fow_texture;

void main() {
    vec4 fow_value = texture(fow_texture, UV);
    vec4 color_value = texture(game_texture, UV);

    color = color_value * fow_value;
}