#version 430

in vec3 color_out;
out vec4 result_color;

void main()
{
    result_color = vec4(color_out, 1.0);
}

