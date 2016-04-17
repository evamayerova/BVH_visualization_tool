#include "colorMapping.h"

/// Maximum color value.
#define MAX_COLOR_VALUE 255
/// Minimum color value.
#define MIN_COLOR_VALUE 0


colorMapping::colorMapping()
{
}

QVector3D colorMapping::setRainbowColor(const float & iValue, const float &localMin, const float &localMax)
{
	QVector3D color;

	float resizedVal = (iValue - localMin) / (localMax - localMin);
	float value = 4.0f*(1.0f - resizedVal);

	if (value < 0.0f)
		value = 0.0f;
	else
		if (value > 4.0f)
			value = 4.0f;

	int band = (int)(value);
	value -= band;

	switch (band) {
	case 0:
		color[0] = static_cast<float>(MAX_COLOR_VALUE);
		color[1] = static_cast<float>(value*MAX_COLOR_VALUE);
		color[2] = static_cast<float>(MIN_COLOR_VALUE);
		break;
	case 1:
		color[0] = static_cast<unsigned char>((1.0f - value)*MAX_COLOR_VALUE);
		color[1] = static_cast<unsigned char>(MAX_COLOR_VALUE);
		color[2] = static_cast<unsigned char>(MIN_COLOR_VALUE);
		break;
	case 2:
		color[0] = static_cast<unsigned char>(MIN_COLOR_VALUE);
		color[1] = static_cast<unsigned char>(MAX_COLOR_VALUE);
		color[2] = static_cast<unsigned char>(value*MAX_COLOR_VALUE);
		break;
	case 3:
		color[0] = static_cast<unsigned char>(MIN_COLOR_VALUE);
		color[1] = static_cast<unsigned char>((1.0f - value)*MAX_COLOR_VALUE);
		color[2] = static_cast<unsigned char>(MAX_COLOR_VALUE);
		break;
	default:
		color[0] = static_cast<unsigned char>(value*MAX_COLOR_VALUE);
		color[1] = static_cast<unsigned char>(MIN_COLOR_VALUE);
		color[2] = static_cast<unsigned char>(MAX_COLOR_VALUE);
		break;
	}

	return color;
}


colorMapping::~colorMapping()
{
}
