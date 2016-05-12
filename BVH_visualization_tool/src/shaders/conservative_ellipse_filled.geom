#version 430
layout(points) in;
layout(triangle_strip, max_vertices = 49) out;

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
	float ang = 0.0;
	vec4 lastOffset = vec4(cos(ang) * halfDims[0][0] * 0.5, -sin(ang) * halfDims[0][1], 0.0, 0.0);

	for (int i = 1; i <= 16; i++) {
        ang = PI * 2.0 / 16.0 * i;
        vec4 offset = vec4(cos(ang) * halfDims[0][0] * 0.5, -sin(ang) * halfDims[0][1], 0.0, 0.0);
		
		if (offset[0] <= 0)
			offset[0] -= hPixel[0];
		else if (offset[0] > 0)
			offset[0] += hPixel[0];

        gl_Position = gl_in[0].gl_Position + lastOffset;
		EmitVertex();
		gl_Position = gl_in[0].gl_Position + offset;
		EmitVertex();
		gl_Position = gl_in[0].gl_Position;
        EmitVertex();
		lastOffset = offset;
		EndPrimitive();
    }

}
