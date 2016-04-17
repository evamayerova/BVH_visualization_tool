#version 430
in vec3 position;
in vec3 color_in;

uniform mat4 mvp_matrix;
out vec3 color_out;

void main()
{
	color_out = color_in;
    gl_Position = mvp_matrix * vec4(position, 1.0);
}

