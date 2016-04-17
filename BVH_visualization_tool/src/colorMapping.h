#pragma once

#include <QVector3D>

struct Color
{
	char color[4];
	/*
	Color(char r, char g, char b, char a)
	{
		color[0] = r;
		color[1] = g;
		color[2] = b;
		color[3] = a;
	};
*/
};

class colorMapping
{
public:
	colorMapping();
	QVector3D setRainbowColor(const float & iValue, const float &localMin, const float &localMax);
	~colorMapping();
};

