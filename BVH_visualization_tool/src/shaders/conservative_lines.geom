#version 430
layout(points) in;
//layout(triangle_strip, max_vertices = 4) out;
layout(line_strip, max_vertices = 5) out;

in float color_vert[];
in vec2 halfDims[];
uniform vec2 hPixel;
out float color_frag;

void main()
{
	color_frag = color_vert[0];
	gl_Position = gl_in[0].gl_Position + vec4(-halfDims[0].x, -halfDims[0].y, 0.0, 0.0);
	EmitVertex();
	gl_Position = gl_in[0].gl_Position + vec4(halfDims[0].x, -halfDims[0].y, 0.0, 0.0);
	EmitVertex();
	gl_Position = gl_in[0].gl_Position + vec4(halfDims[0].x, halfDims[0].y, 0.0, 0.0);
	EmitVertex();
	gl_Position = gl_in[0].gl_Position + vec4(-halfDims[0].x, halfDims[0].y, 0.0, 0.0);
	EmitVertex();
	gl_Position = gl_in[0].gl_Position + vec4(-halfDims[0].x, -halfDims[0].y, 0.0, 0.0);
	EmitVertex();
	EndPrimitive();
}