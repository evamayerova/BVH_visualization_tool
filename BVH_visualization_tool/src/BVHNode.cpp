#include "BVHNode.h"

const bool BVHNode::inside(QVector3D point) const
{
	if (point.x() >= bounds[0][0] && point.y() >= bounds[0][1] && point.z() >= bounds[0][2] &&
		point.x() <= bounds[1][0] && point.y() <= bounds[1][1] && point.z() <= bounds[1][2])
		return true;
	return false;
}

const bool BVHNode::intersect(ray & r) const
{
	float tNear = -std::numeric_limits<float>::infinity();
	float tFar = std::numeric_limits<float>::infinity();

	float t1, t2;
	for (int i = 0; i < 3; i++) {
		// ray is parallel to X-plane
		if (r.dir[i] == 0) {
			if (r.orig[i] < bounds[0][i] || r.orig[i] > bounds[1][i])
				return false;
		}
		// ray is not parallel to X-plane
		else {
			t1 = (bounds[0][i] - r.orig[i]) * r.dirfrac[i];
			t2 = (bounds[1][i] - r.orig[i]) * r.dirfrac[i];

			if (t1 > t2) {
				float tmp = t1;
				t1 = t2;
				t2 = tmp;
			}
			if (t1 > tNear)
				tNear = t1;

			if (t2 < tFar)
				tFar = t2;

			if (tNear > tFar)
				return false;
			if (tFar < 0)
				return false;
		}
	}
	return true;
}
