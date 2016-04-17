#pragma once

#include "Scene.h"
#include "bvh.h"
#include <fstream>
#include <iostream>
#include <sstream>


class SceneImporter
{
public:
	SceneImporter(BVH *b, Scene *s) : bvh(b), sc(s) {}
	~SceneImporter();

	void loadFromBinaryFile(const string& fileName);
	bool loadScalars(const string &fileName);

	BVH *bvh;
	Scene *sc;
	colorMapping cm;
};

