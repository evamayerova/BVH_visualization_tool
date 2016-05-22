#include "SceneImporter.h"


SceneImporter::~SceneImporter()
{
}

//-------------------------------------------------
// author of this part: Marek Vinkler, January 2015
void SceneImporter::loadFromBinaryFile(const std::string& fileName)
{
	ifstream in(fileName, ios::binary);

	if (!in) {
		qDebug() << "Cannot open import file" << endl;
		throw "Could not open file " + fileName;
	}

	// skip layouts
	uint32_t toSkip;
	in.read(reinterpret_cast<char*>(&toSkip), sizeof(uint32_t));
	in.read(reinterpret_cast<char*>(&toSkip), sizeof(uint32_t));
	in.read(reinterpret_cast<char*>(&toSkip), sizeof(uint32_t));

	// read builder name
	in.read(reinterpret_cast<char*>(&toSkip), sizeof(uint32_t));
	char* buffer = new char[toSkip];
	in.read(buffer, toSkip*sizeof(char));
	bvh->builderName = string(buffer);
	delete[] buffer;

	// Read triangles
	uint32_t geometrySize;
	in.read(reinterpret_cast<char*>(&geometrySize), sizeof(uint32_t));
	sc->mTriangles.resize(geometrySize);
	in.read(reinterpret_cast<char*>(sc->mTriangles.data()),
		geometrySize * sizeof(Triangle));

	// Read indices  
	uint32_t indexSize;
	vector<unsigned int> *indices = new vector<unsigned int>();
	in.read(reinterpret_cast<char*>(&indexSize), sizeof(uint32_t));
	//sc->mTriangleIdx.resize(indexSize);
	indices->resize(indexSize);
	in.read(reinterpret_cast<char*>(indices->data()),
		indexSize * sizeof(unsigned int));
	sc->mTriangleIdx.push_back(*indices);

	// Read nodes
	uint32_t nodeSize;
	in.read(reinterpret_cast<char*>(&nodeSize), sizeof(uint32_t));
	bvh->mNodes.resize(nodeSize);
	in.read(reinterpret_cast<char*>(bvh->mNodes.data()),
		nodeSize * sizeof(BVHNode));


	in.close();
}

//-----------------------------------------

bool SceneImporter::loadScalars(const string &fileName)
{
	std::ifstream inFile(fileName, ios::binary);

	if (!inFile)
		throw "Input file not created";

	uint32_t actualNodeSize;
	inFile.read(reinterpret_cast<char*>(&actualNodeSize), sizeof(uint32_t));
	if (actualNodeSize != bvh->mMeshCenterCoordinatesNr)
		return false;

	uint8_t colorSetSize;
	inFile.read(reinterpret_cast<char*>(&colorSetSize), sizeof(uint8_t));

	size_t nameLen;
	char *setName;
	for (uint8_t i = 0; i < colorSetSize; i++)
	{
		setName = NULL;
		inFile.read(reinterpret_cast<char*>(&nameLen), sizeof(size_t));
		setName = new char[nameLen];
		inFile.read(setName, nameLen*sizeof(char));

		if (strcmp(setName, "area") == 0)
		{
			vector<float> areas;
			areas.resize(actualNodeSize);
			inFile.read(reinterpret_cast<char*>(areas.data()), actualNodeSize * sizeof(float));
			for (uint32_t j = 0; j < actualNodeSize; j++)
			{
				if (areas[j] != bvh->mBoxSizes[j])
					return false;
			}
			areas.clear();
		}
		else
		{
			ScalarSet *s = new ScalarSet();
			s->name.assign(setName, nameLen);
			s->colors.resize(actualNodeSize);
			inFile.read(reinterpret_cast<char*>(s->colors.data()), actualNodeSize * sizeof(float));
			bvh->mScalarSets.push_back(s);
			bvh->normalizeScalarSet(bvh->mScalarSets.size() - 1);
		}
		delete[] setName;
	}
	
	inFile.close();
	return true;
}

bool SceneImporter::loadBVH(BVH *b, const string & fileName)
{
	ifstream in(fileName, ios::binary);

	if (!in) {
		qDebug() << "Cannot open import file" << endl;
		throw "Could not open file " + fileName;
	}

	// skip layouts
	uint32_t toSkip;
	in.read(reinterpret_cast<char*>(&toSkip), sizeof(uint32_t));
	in.read(reinterpret_cast<char*>(&toSkip), sizeof(uint32_t));
	in.read(reinterpret_cast<char*>(&toSkip), sizeof(uint32_t));

	// read builder name
	in.read(reinterpret_cast<char*>(&toSkip), sizeof(uint32_t));
	char* buffer = new char[toSkip];
	in.read(buffer, toSkip*sizeof(char));
	b->builderName = string(buffer);
	delete[] buffer;

	// Read triangles
	uint32_t geometrySize;
	vector<Triangle> tmpTriangles;
	in.read(reinterpret_cast<char*>(&geometrySize), sizeof(uint32_t));
	tmpTriangles.resize(geometrySize);
	in.read(reinterpret_cast<char*>(tmpTriangles.data()),
		geometrySize * sizeof(Triangle));

	if (memcmp(tmpTriangles.data(), sc->mTriangles.data(), geometrySize * sizeof(Triangle)) != 0)
		return false;

	// Read indices  
	uint32_t indexSize;
	vector<unsigned int> *indices = new vector<unsigned int>();
	in.read(reinterpret_cast<char*>(&indexSize), sizeof(uint32_t));
	indices->resize(indexSize);
	in.read(reinterpret_cast<char*>(indices->data()),
		indexSize * sizeof(unsigned int));
	sc->mTriangleIdx.push_back(*indices);

	// Read nodes
	uint32_t nodeSize;
	in.read(reinterpret_cast<char*>(&nodeSize), sizeof(uint32_t));
	b->mNodes.resize(nodeSize);
	in.read(reinterpret_cast<char*>(b->mNodes.data()),
		nodeSize * sizeof(BVHNode));

	in.close();
	return true;
}
