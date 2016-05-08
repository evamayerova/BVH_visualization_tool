#version 430
layout(points) in;
layout(line_strip, max_vertices = 18) out;

in float color_vert[];
in vec2 halfDims[];
uniform vec2 hPixel;
out vec4 frag_position;
out float color_frag;
const float PI = 3.1415926;

void main()
{
	frag_position = gl_in[0].gl_Position;
	color_frag = color_vert[0];
	
	for (int i = 0; i <= 16; i++) {
        float ang = PI * 2.0 / 16.0 * i;
        vec4 offset = vec4(cos(ang) * halfDims[0][0] * 0.5, -sin(ang) * halfDims[0][1], 0.0, 0.0);
        gl_Position = gl_in[0].gl_Position + offset;

        EmitVertex();
    }

	float ang = 0;
	vec4 offset = vec4(cos(ang) * halfDims[0][0] * 0.5, -sin(ang) * halfDims[0][1], 0.0, 0.0);
    gl_Position = gl_in[0].gl_Position + offset;
	EmitVertex();

    EndPrimitive();
}
