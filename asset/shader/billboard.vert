#version 330 core

layout(location = 0) in vec3 vertex_position_modelspace;
layout(location = 1) in vec3 vertex_color;
layout(location = 2) in vec2 vertex_uv;

// Output data ; will be interpolated for each fragment.
out vec3 fragment_color;
out vec2 UV;

// Values that stay constant for the whole mesh.
uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 projection_matrix;

void main() {
    mat4 model_view_matrix = view_matrix * model_matrix;

    // Cancels rotation
    model_view_matrix[0][0] = 1.0;
    model_view_matrix[0][1] = 0.0;
    model_view_matrix[0][2] = 0.0;

    model_view_matrix[1][0] = 0.0;
    model_view_matrix[1][1] = 1.0;
    model_view_matrix[1][2] = 0.0;

    model_view_matrix[2][0] = 0.0;
    model_view_matrix[2][1] = 0.0;
    model_view_matrix[2][2] = 1.0;

    // Calculate position
    vec4 p = model_view_matrix * vec4(vertex_position_modelspace, 1.0);
    gl_Position = projection_matrix * p;

    // Setup UV and color
    fragment_color = vertex_color;
    UV = vertex_uv;
}
