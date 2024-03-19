#pragma once
#include "mvmath.h"
#include <vector>
#include <string>

class Mesh
{
public:
	Mesh(std::vector<triangle> tri);


	bool BuildMeshFromObjFile(std::string);
private:
	triangle tri;
};
