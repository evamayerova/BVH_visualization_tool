#version 430
layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in vec3 color_vert[];
uniform vec2 hPixel;
out vec3 color_frag;

void main()
{
	color_frag = color_vert[0];
	
	for (int i = 0; i < 3; i ++)
	{
		if (gl_in[i].gl_Position.x < gl_in[(i+1)%3].gl_Position.x || 
			gl_in[i].gl_Position.x < gl_in[(i-1)%3].gl_Position.x)
		{
			gl_Position = gl_in[i].gl_Position - vec4(hPixel.x * 1.5, 0.0, 0.0, 0.0);
			EmitVertex();
		}
		else {
			gl_Position = gl_in[i].gl_Position;
			EmitVertex();
		}
	}
	EndPrimitive();
}
