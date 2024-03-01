#pragma once
#include <vector>
#include <string>
#include "mvmath.h"

class Mesh
{
public:
	Mesh(std::vector<triangle>);


	bool BuildMeshFromObjFile(std::string);
};
