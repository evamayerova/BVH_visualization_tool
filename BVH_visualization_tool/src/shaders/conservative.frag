#version 430

in vec4 gl_FragCoord;
in vec3 color_frag;
out vec4 result_color;

void main()
{
/*
	vec2 pos = gl_FragCoord.xy / gl_FragCoord.w;
	if (pos.x < AABB[0].x || pos.y < AABB[0].y || 
		pos.x > AABB[1].x || pos.y > AABB[1].y)
		discard;
*/
    result_color = vec4(color_frag, 1.0);
}

