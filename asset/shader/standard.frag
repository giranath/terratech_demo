#version 330 core

// Interpolated values from the vertex shaders
in vec3 fragment_color;
in vec2 UV;

// Ouput data
out vec3 color;

uniform sampler2D texture_sampler;
uniform int is_textured;

void main(){
    color = (is_textured * texture(texture_sampler, UV).rgb) * fragment_color + (1 - is_textured) * fragment_color;
}