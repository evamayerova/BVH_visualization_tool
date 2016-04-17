#version 430

in vec3 position, normal, color_in;
uniform mat4 model, mvp_matrix;
out vec3 color_out, pos_out, nor_out;

void main()
{
    pos_out = vec4(model * vec4(position, 1.0)).xyz;
    mat3 normal_matrix = transpose(inverse(mat3(model)));
    nor_out = normal_matrix * normal;
	color_out = color_in;
    gl_Position = mvp_matrix * vec4(position, 1.0);
}

