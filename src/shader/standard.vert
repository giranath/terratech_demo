#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertex_position_modelspace;
layout(location = 1) in vec3 vertex_color;

// Output data ; will be interpolated for each fragment.
out vec3 fragment_color;

// Values that stay constant for the whole mesh.
uniform mat4 model_matrix;
uniform mat4 camera_matrix;

void main(){
    mat4 mvp = camera_matrix * model_matrix;

    gl_Position =  mvp * vec4(vertex_position_modelspace, 1);
    fragment_color = vertex_color;
}