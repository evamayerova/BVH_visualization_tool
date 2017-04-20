#version 430
layout(location = 0)in vec3 position;
layout(location = 1)in float half_node_width;
layout(location = 2)in float half_node_height;
layout(location = 3)in float color_in;

uniform mat4 mvp_matrix;

out float color_vert;
out vec2 halfDims;
out mat4 mvp;

void main()
{
	halfDims = vec2(half_node_width, half_node_height);
	color_vert = color_in;
	mvp = mvp_matrix;
    gl_Position = mvp_matrix * vec4(position, 1.0);
}

