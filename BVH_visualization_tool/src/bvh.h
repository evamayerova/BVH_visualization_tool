#ifndef BVH_H
#define BVH_H

#include "colorMapping.h"
#include "Mesh.h"
#include "BVHNode.h"

#include <vector>
#include <fstream>
#include <QVector2D>
#include <QVector3D>

#define MAX_TREE_DEPTH 10000

using namespace std;

struct ScalarSet {
	string name;
	vector<float> colors;
	float localMin, localMax, selectedMin, selectedMax;
};

struct BBox {
	QVector3D extremes[2];
};

class BVH
{
public:
    BVH();
    ~BVH();
	unsigned getTriangleCount(int index);
    void generateTree(int maxDepth = MAX_TREE_DEPTH);
	void setDefaultScalars();
	void normalizeScalarSets();
	void normalizeScalarSet(int index);
	void normalizeScalarSet(int index, const float & resizedMin, const float & resizedMax);

	vector<BVHNode> mNodes; // array of imported BVH nodes
	vector<QVector2D> mPositions; // screen-space positions of mesh nodes
	vector<unsigned int> mNodeDepths; // depths of BVH nodes
	vector<unsigned int> mNodeParents; // parents of BVH nodes
	vector<unsigned> mMeshToBVHIndices; // indices used in mesh mapped to real indices in BVH
	vector<unsigned> mBVHToMeshIndices; // BVH indices mapped to mesh indices (BVS)
	vector<float> mBoxSizes; // bounding-box sizes
	vector<BBox> meshExtremes; // bounding-box extremes for mesh nodes
	vector<ScalarSet*> mScalarSets; // imported scalar value sets
	float *mMeshCenterCoordinates; // array of computed coordinates of node centers in mesh

    int depth, arity, mMeshCenterCoordinatesNr;
	string builderName;

private:
    //void generateMeshes();
	void generateRealNodesBFS(int maxDepth = MAX_TREE_DEPTH);
};

#endif // BVH_H
