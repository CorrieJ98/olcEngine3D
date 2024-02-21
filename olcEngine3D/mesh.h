#pragma once
#include <string>
#include <vector>
#include "vector.cpp"

#ifndef MESH_H
#define MESH_H

class Mesh
{
public:
	Mesh(std::vector<triangle>);


	bool BuildMeshFromObjFile(std::string);

protected:
private:

};
#endif // !MESH_H
