#version 430
layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

in float color_vert[];
in vec2 halfDims[];
uniform vec2 hPixel;
out vec4 frag_position;
out float color_frag;

void main()
{
	frag_position = gl_in[0].gl_Position;
	color_frag = color_vert[0];
	gl_Position = gl_in[0].gl_Position + vec4(-halfDims[0].x - hPixel[0], -halfDims[0].y, 0.0, 0.0);
	EmitVertex();
	gl_Position = gl_in[0].gl_Position + vec4(halfDims[0].x + hPixel[0], -halfDims[0].y, 0.0, 0.0);
	EmitVertex();
	gl_Position = gl_in[0].gl_Position + vec4(-halfDims[0].x - hPixel[0], halfDims[0].y, 0.0, 0.0);
	EmitVertex();
	gl_Position = gl_in[0].gl_Position + vec4(halfDims[0].x + hPixel[0], halfDims[0].y, 0.0, 0.0);
	EmitVertex();
	EndPrimitive();
}
