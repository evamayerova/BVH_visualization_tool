#version 430
layout(location = 0)in float position;
layout(location = 1)in vec2 scalar_val;

out vec2 scalar_vert;

void main()
{
	scalar_vert = scalar_val;
    gl_Position = vec4(1.0, position, 0.0, 1.0);
}

