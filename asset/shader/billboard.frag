#version 330

// Interpolated values from the vertex shaders
in vec4 fragment_color;
in vec2 UV;

// Ouput data
layout(location = 0) out vec4 color;

uniform sampler2D texture_sampler;
uniform int is_textured;

void main(){
    vec4 texture_color = texture(texture_sampler, UV);

    color = (is_textured * texture_color) * fragment_color + (1 - is_textured) * fragment_color;
}