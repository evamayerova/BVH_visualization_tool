#version 430

in float color_frag;
in vec4 frag_position;

uniform float scalar_min, scalar_max;

out vec4 output_value;

void main()
{
    output_value = vec4(vec3((color_frag - scalar_min) / (scalar_max - scalar_min)), 1.0);
}

