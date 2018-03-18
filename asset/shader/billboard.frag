#version 330

// Interpolated values from the vertex shaders
in vec3 fragment_color;
in vec2 UV;

// Ouput data
out vec4 color;

uniform sampler2D texture_sampler;
uniform int is_textured;

void main(){
    vec4 frag_color = vec4(fragment_color, 1.0);
    vec4 texture_color = texture(texture_sampler, UV);

    if(texture_color.a < 0.1)
        discard;

    color = (is_textured * texture_color) * frag_color + (1 - is_textured) * frag_color;
}