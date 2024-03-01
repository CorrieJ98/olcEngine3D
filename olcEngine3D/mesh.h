#pragma once
#include "include.h"

class Mesh
{
public:
	Mesh(std::vector<triangle> t_tris);


	void BuildMeshFromObjFile(std::string);
};
