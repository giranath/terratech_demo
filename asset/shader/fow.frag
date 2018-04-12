#version 330

// Interpolated values from the vertex shaders
in vec4 fragment_color;

// Ouput data
out vec4 color;

void main(){
    color = fragment_color;
}