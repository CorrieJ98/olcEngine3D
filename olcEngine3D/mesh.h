#pragma once
#include "mvmath.h"
#include <vector>
#include <string>

class Mesh
{
public:
	Mesh(std::vector<triangle> triangle);


	bool BuildMeshFromObjFile(std::string);
};
