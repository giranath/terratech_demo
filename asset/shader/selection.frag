#version 330

// Interpolated values from the vertex shaders
in vec4 fragment_color;
in vec2 UV;

// Ouput data
out vec4 color;

uniform sampler2D texture_sampler;
uniform int is_textured;

void main(){
    color = (is_textured * texture(texture_sampler, UV)) * fragment_color + (1 - is_textured) * fragment_color;
}