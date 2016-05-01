#pragma once

#include "Scene.h"
#include <cstdint>
#include <QVector3D>

struct ray
{
	QVector3D orig, dir, norm, dirfrac;
	/** Parameter, which indicates the distance of the intersected object */
	float t;
	int BVHListNode;
	ray(const QVector3D & o, const QVector3D & d) : orig(o), dir(d), t(std::numeric_limits<float>::infinity()), BVHListNode(-1)
	{
		dirfrac[0] = 1.f / d[0];
		dirfrac[1] = 1.f / d[1];
		dirfrac[2] = 1.f / d[2];
	}
};

typedef struct
{
	float bounds[2][3];     ///< Minumum and maximum floating-point point of the AABB.
							/** The left child node (if axis<3) or primitive (if axis == 3) index;
							the right child must be equal to 'child' + 1. */
	int32_t child;
	uint8_t axis;           ///< {0,1,2}=interior node split axis {x,y,z}, 3=leaf node.

	uint8_t isLeftCheaper;  ///< Unused.
							/** The number of child nodes (if axis<3) or primitives (if axis==3);
							An interior node must have two children. */
	int16_t children;

	bool IsLeaf() const {
		return axis == 3;
	}

	float GetBoxVolume() const {
		return ((bounds[1][0] - bounds[0][0]) *
				(bounds[1][1] - bounds[0][1]) *
				(bounds[1][2] - bounds[0][2])
			);
	}

	float GetBoxArea() const {
		float a = (bounds[1][0] - bounds[0][0]);
		float b = (bounds[1][1] - bounds[0][1]);
		float c = (bounds[1][2] - bounds[0][2]);

		return (2 * a * b +
			2 * a * c +
			2 * b * c
			);
	}

	const bool intersect(ray & r) const;
	const bool inside(QVector3D point) const;

} BVHNode;