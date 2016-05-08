#include "bvh.h"

#include <queue>
#include <time.h>

#define MAX_MESH_WIDTH 0.2

BVH::BVH()
{
	mMeshCenterCoordinates = NULL;
	mScalarSets.clear();
}

BVH::~BVH()
{
	if (mNodes.size() == 0)
		return;

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

		mBoxSizes.push_back(mNodes[currIndex].GetBoxArea());

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
	
	/*
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
		a->colors[i] = mNodes[mMeshToBVHIndices[i]].GetBoxArea() *
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
	*/

	float sum;
	ScalarSet *area = new ScalarSet();
	area->name = "area";
	area->colors.resize(mMeshCenterCoordinatesNr);
	for (unsigned i = 0; i < mMeshCenterCoordinatesNr; i++)
	{
		area->colors[i] = mBoxSizes[i];
	}
	mScalarSets.push_back(area);

	ScalarSet *volume = new ScalarSet();
	volume->name = "volume";
	volume->colors.resize(mMeshCenterCoordinatesNr);
	for (unsigned i = 0; i < mMeshCenterCoordinatesNr; i++)
	{
		volume->colors[i] = mNodes[mMeshToBVHIndices[i]].GetBoxVolume();
	}
	mScalarSets.push_back(volume);

	// area relatively to depth
	ScalarSet *a = new ScalarSet();
	a->name = "area relative to depth";
	a->colors.resize(mMeshCenterCoordinatesNr);
	for (unsigned i = 0; i < mMeshCenterCoordinatesNr; i++)
	{
		a->colors[i] = mNodes[mMeshToBVHIndices[i]].GetBoxArea() *
			pow(2, mNodeDepths[mMeshToBVHIndices[i]]);
	}
	mScalarSets.push_back(a);

	// volume relatively to depth
	ScalarSet *volumeRelativeDepth = new ScalarSet();
	volumeRelativeDepth->name = "volume relative to depth";
	volumeRelativeDepth->colors.resize(mMeshCenterCoordinatesNr);
	for (unsigned i = 0; i < mMeshCenterCoordinatesNr; i++)
	{
		volumeRelativeDepth->colors[i] = mNodes[mMeshToBVHIndices[i]].GetBoxVolume() *
			pow(2, mNodeDepths[mMeshToBVHIndices[i]]);
	}
	mScalarSets.push_back(volumeRelativeDepth);

	// sum of children area relative to parent area
	ScalarSet *areaVsParent = new ScalarSet();
	areaVsParent->name = "sum of children area relative to parent area";
	areaVsParent->colors.resize(mMeshCenterCoordinatesNr);
	for (unsigned i = 0; i < mMeshCenterCoordinatesNr; i++)
	{
		sum = 0;
		if (mNodes[mMeshToBVHIndices[i]].axis != 3)
		{
			for (unsigned j = 0; j < mNodes[mMeshToBVHIndices[i]].children; j++)
				sum += mNodes[mNodes[mMeshToBVHIndices[i]].child + j].GetBoxArea();
		}
		qDebug() << sum /
			(mNodes[mMeshToBVHIndices[i]].children * mNodes[mMeshToBVHIndices[i]].GetBoxArea());
		areaVsParent->colors[i] = sum /
			(mNodes[mMeshToBVHIndices[i]].children * mNodes[mMeshToBVHIndices[i]].GetBoxArea());
	}
	mScalarSets.push_back(areaVsParent);

	// sum of children volume relative to parent volume
	ScalarSet *volumeVsParent = new ScalarSet();
	volumeVsParent->name = "sum of children volume relative to parent volume";
	volumeVsParent->colors.resize(mMeshCenterCoordinatesNr);
	for (unsigned i = 0; i < mMeshCenterCoordinatesNr; i++)
	{
		sum = 0;
		if (mNodes[mMeshToBVHIndices[i]].axis != 3)
		{
			for (unsigned j = 0; j < mNodes[mMeshToBVHIndices[i]].children; j++)
				sum += mNodes[mNodes[mMeshToBVHIndices[i]].child + j].GetBoxVolume();
		}
		volumeVsParent->colors[i] = sum /
			(mNodes[mMeshToBVHIndices[i]].children * mNodes[mMeshToBVHIndices[i]].GetBoxVolume());
	}
	mScalarSets.push_back(volumeVsParent);

	/*
	ScalarSet *childSimilarity = new ScalarSet();
	childSimilarity->name = "children-volume-similarity rate";
	childSimilarity->colors.resize(mMeshCenterCoordinatesNr);
	for (unsigned i = 0; i < mMeshCenterCoordinatesNr; i++)
	{
		sum = 0;
		if (mNodes[i].axis != 3)
		{
			sum = abs(mNodes[mNodes[mBVHToMeshIndices[i]].child].GetBoxVolume() - 
				mNodes[mNodes[mBVHToMeshIndices[i]].child + 1].GetBoxVolume());
		}
		assert(mNodes[mMeshToBVHIndices[i]].GetBoxVolume() > 0);
		childSimilarity->colors[i] = sum / mNodes[mMeshToBVHIndices[i]].GetBoxVolume();
	}
	mScalarSets.push_back(childSimilarity);
	*/

	ScalarSet *b = new ScalarSet();
	b->name = "triangle number";
	b->colors.resize(mMeshCenterCoordinatesNr);
	for (unsigned i = 0; i < mMeshCenterCoordinatesNr; i++)
	{
		b->colors[i] = getTriangleCount(mMeshToBVHIndices[i]);
	}
	mScalarSets.push_back(b);

	normalizeScalarSets();
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

