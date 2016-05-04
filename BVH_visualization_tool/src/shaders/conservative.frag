#version 430

in float color_frag;
in vec4 frag_position;

layout(binding = 1) uniform sampler2D scalarValues;
uniform float colorMapMin, colorMapMax;
uniform float winWidth, winHeight;

out vec4 result_color;

vec3 colorMapping (float inValue, float localMin, float localmax)
{
	vec3 result;
	
	if (inValue == -1.0)
		return vec3(1.0);

	float max_val = 1.0;
	float min_val = 0.0;

	float resizedVal = (inValue - localMin) / (localmax - localMin);
	if (resizedVal > 1)
		return vec3(1.0);

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
		result.x = max_val;
		result.y = value;
		result.z = min_val;
		break;
	case 1:
		result.x = 1.0f - value;
		result.y = max_val;
		result.z = min_val;
		break;
	case 2:
		result.x = min_val;
		result.y = max_val;
		result.z = value;
		break;
	case 3:
		result.x = min_val;
		result.y = 1.0f - value;
		result.z = max_val;
		break;
	default:
		result.x = value;
		result.y = min_val;
		result.z = max_val;
		break;
	}

	return result;
}

void main()
{
    vec4 texColor = texture(scalarValues, vec2((frag_position.x + 1) / 2.0, (frag_position.y + 1) / 2.0));
	
	if (color_frag == -1.0)
		result_color = vec4(1.0);
	else
		result_color = vec4(colorMapping(texColor.r, colorMapMin, colorMapMax), 1.0);
}

