#version 430
layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

in vec2 scalar_vert[];
out float scalar_frag;

void main()
{
	float height = 2 / 99.;
	scalar_frag = scalar_vert[0].x;
	gl_Position = gl_in[0].gl_Position + vec4(-0.05, 0.0, 0.0, 0.0);
	EmitVertex();
	scalar_frag = scalar_vert[0].x;
	gl_Position = gl_in[0].gl_Position + vec4(0.0, 0.0, 0.0, 0.0);
	EmitVertex();
	scalar_frag = scalar_vert[0].y;
	gl_Position = gl_in[0].gl_Position + vec4(-0.05, height, 0.0, 0.0);
	EmitVertex();
	scalar_frag = scalar_vert[0].y;
	gl_Position = gl_in[0].gl_Position + vec4(0.0, height, 0.0, 0.0);
	EmitVertex();

	EndPrimitive();
}
