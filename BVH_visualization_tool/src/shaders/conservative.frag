#version 430

in vec4 gl_FragCoord;
in float color_frag;

uniform float colorMapMin, colorMapMax;

out vec4 result_color;

vec3 colorMapping (float inValue, float localMin, float localMax)
{
	vec3 result;
	
	if (inValue == -1.0)
		return vec3(1.0);

	float max = 1.0;
	float min = 0.0;

	float resizedVal = (inValue - localMin) / (localMax - localMin);
	float value = 4.0f * (1.0f - resizedVal);

	if (value < 0.0f)
		value = 0.0f;
	else
		if (value > 4.0f)
			value = 4.0f;

	int band = int(value);
	value -= band;

	switch (band) {
	case 0:
		result.x = max;
		result.y = value;
		result.z = min;
		break;
	case 1:
		result.x = 1.0f - value;
		result.y = max;
		result.z = min;
		break;
	case 2:
		result.x = min;
		result.y = max;
		result.z = value;
		break;
	case 3:
		result.x = min;
		result.y = 1.0f - value;
		result.z = max;
		break;
	default:
		result.x = value;
		result.y = min;
		result.z = max;
		break;
	}

	return result;
}

void main()
{
/*
	vec2 pos = gl_FragCoord.xy / gl_FragCoord.w;
	if (pos.x < AABB[0].x || pos.y < AABB[0].y || 
		pos.x > AABB[1].x || pos.y > AABB[1].y)
		discard;
*/
    result_color = vec4(colorMapping(color_frag, colorMapMin, colorMapMax), 1.0);
}

