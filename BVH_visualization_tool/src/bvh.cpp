#include "bvh.h"

#include <queue>
#include <time.h>

#define MAX_MESH_WIDTH 0.2

BVH::BVH()
{
	mMeshCenterCoordinates = NULL;
}

BVH::~BVH()
{
	if (mMeshCenterCoordinates)
	{
		delete[] mMeshCenterCoordinates;
		mMeshCenterCoordinates = NULL;
	}

	for (int i = 0; i < mScalarSets.size(); i++)
	{
		delete mScalarSets[i];
		mScalarSets[i] = NULL;
	}

	mNodes.clear();
	mPositions.clear();
	mNodeDepths.clear();
	mNodeParents.clear();
	mMeshToBVHIndices.clear();
	mBVHToMeshIndices.clear();
	mBoxSizes.clear();
	meshExtremes.clear();
	mScalarSets.clear();
}

unsigned BVH::getTriangleCount(int index)
{
	unsigned count = 0;
	BVHNode *node = &mNodes[index];
	if (node->axis == 3) {
		return node->children;
	}
	return getTriangleCount(node->child) + getTriangleCount(node->child + 1);
}

/*
 * BFS pruchod stromem
 */
void BVH::generateRealNodesBFS(int maxDepth)
{
	mNodeDepths.resize(mNodes.size());
	mNodeParents.resize(mNodes.size());
	mPositions.resize(mNodes.size());
	mBVHToMeshIndices.resize(mNodes.size());

	mNodeDepths[0] = 0;
	mNodeParents[0] = -1;
	depth = 0;


	std::queue<int> nodeQueue;
	nodeQueue.push(0);

	int actualDepth = 0;
	int nodesAtFloor = 1;
	int arrayPositionCounter = 0;
	int ctr = 0;
	while (!nodeQueue.empty())
	{
		int currIndex = nodeQueue.front();
		nodeQueue.pop();

		if (!mNodes[currIndex].IsLeaf())
		{
			for (int i = 0; i < mNodes[currIndex].children; i++)
			{
				mNodeParents[mNodes[currIndex].child + i] = currIndex;
				mNodeDepths[mNodes[currIndex].child + i] = mNodeDepths[currIndex] + 1;
				nodeQueue.push(mNodes[currIndex].child + i);
			}
		}
		depth = mNodeDepths[currIndex];
		ctr++;
		if (depth > maxDepth) {
			nodeQueue = std::queue<int>();
			break;
		}
	}

	nodeQueue.push(0);
	mMeshCenterCoordinatesNr = 0;
	unsigned i, childrenSize;

	while (!nodeQueue.empty())
	{
		int currIndex = nodeQueue.front();
		nodeQueue.pop();

		if (mNodeDepths[currIndex] > maxDepth) {
			break;
		}

		if (actualDepth < mNodeDepths[currIndex]) {
			mNodes[mNodeParents[currIndex]];
			mNodeDepths[mNodeParents[currIndex]];
			actualDepth++;
			nodesAtFloor *= arity;
		}

		float halfMeshWidth = 1 / float(nodesAtFloor);
		
		if (currIndex == 0)
		{
			mPositions[currIndex][0] = 0;
		}
		else
		{
			mPositions[currIndex][0] = mPositions[mNodeParents[currIndex]][0] - halfMeshWidth * arity + 
				halfMeshWidth * ((currIndex - mNodes[mNodeParents[currIndex]].child + 1) * 2 - 1);
		}

		mPositions[currIndex][1] = 1 - (actualDepth * 2 + 1) / float(depth);

		mMeshCenterCoordinates[arrayPositionCounter++] = mPositions[currIndex][0];
		mMeshCenterCoordinates[arrayPositionCounter++] = mPositions[currIndex][1];

		mMeshToBVHIndices.push_back(currIndex);
		mBVHToMeshIndices[currIndex] = mMeshCenterCoordinatesNr;

		mBoxSizes.push_back(mNodes[currIndex].GetBoxSize());

		mMeshCenterCoordinatesNr++;

		if (!mNodes[currIndex].IsLeaf()) {
			for (i = 0, childrenSize = mNodes[currIndex].children; i < childrenSize; i++)
			{
					nodeQueue.push(mNodes[currIndex].child + i);
			}
		}
	}
}

void BVH::generateTree(int maxDepth)
{
    arity = 2;

    mMeshCenterCoordinates = NULL;
    mMeshCenterCoordinates = new float[mNodes.size() * 2];

    generateRealNodesBFS(maxDepth);
}

void BVH::setDefaultScalars()
{
	for (vector<ScalarSet*>::iterator it = mScalarSets.begin(); it != mScalarSets.end(); it++)
	{
		delete *it;
		*it = NULL;
	}
	mScalarSets.clear();

	ScalarSet *area = new ScalarSet();
	area->name = "area";
	area->colors.resize(mMeshCenterCoordinatesNr);
	for (unsigned i = 0; i < mMeshCenterCoordinatesNr; i++)
	{
		area->colors[i] = mBoxSizes[i];
	}
	mScalarSets.push_back(area);

	ScalarSet *a = new ScalarSet();
	a->name = "relative area";
	a->colors.resize(mMeshCenterCoordinatesNr);
	for (unsigned i = 0; i < mMeshCenterCoordinatesNr; i++)
	{
		a->colors[i] = mNodes[mMeshToBVHIndices[i]].GetBoxSize() *
			pow(2, mNodeDepths[mMeshToBVHIndices[i]]);
	}
	mScalarSets.push_back(a);

	ScalarSet *b = new ScalarSet();
	b->name = "triangle number";
	b->colors.resize(mMeshCenterCoordinatesNr);
	for (unsigned i = 0; i < mMeshCenterCoordinatesNr; i++)
	{
		b->colors[i] = getTriangleCount(mMeshToBVHIndices[i]);
	}
	mScalarSets.push_back(b);
}

void BVH::normalizeScalarSets()
{
	for (unsigned i = 0; i < mScalarSets.size(); i ++)
	{
		mScalarSets[i]->localMin = mScalarSets[i]->localMax = mScalarSets[i]->colors[0];
		for (vector<float>::iterator it = mScalarSets[i]->colors.begin(); it != mScalarSets[i]->colors.end(); it++)
		{
			if (*it > mScalarSets[i]->localMax)
				mScalarSets[i]->localMax = *it;

			else if (*it < mScalarSets[i]->localMin)
				mScalarSets[i]->localMin = *it;
		}

		mScalarSets[i]->selectedMin = mScalarSets[i]->localMin;
		mScalarSets[i]->selectedMax = mScalarSets[i]->localMax;
	}
}

void BVH::normalizeScalarSet(int index)
{
	mScalarSets[index]->localMin = mScalarSets[index]->localMax = mScalarSets[index]->colors[0];
	for (vector<float>::iterator it = mScalarSets[index]->colors.begin(); it != mScalarSets[index]->colors.end(); it++)
	{
		if (*it > mScalarSets[index]->localMax)
			mScalarSets[index]->localMax = *it;

		else if (*it < mScalarSets[index]->localMin)
			mScalarSets[index]->localMin = *it;
	}

	mScalarSets[index]->selectedMin = mScalarSets[index]->localMin;
	mScalarSets[index]->selectedMax = mScalarSets[index]->localMax;
}

void BVH::normalizeScalarSet(int index, const float & resizedMin, const float & resizedMax)
{
	assert(index < mScalarSets.size());
	assert(resizedMin >= mScalarSets[index]->localMin);
	assert(resizedMax <= mScalarSets[index]->localMax);

	mScalarSets[index]->selectedMin = resizedMin;
	mScalarSets[index]->selectedMax = resizedMax;
}

