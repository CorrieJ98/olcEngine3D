#include "include.h"

using namespace std;

vector<triangle> tris;

Mesh::Mesh(vector<triangle> t_tris) {

	tris = t_tris;
}

void Mesh::BuildMeshFromObjFile(std::string filename)
{
	std::ifstream file(filename);
	if (!file.is_open())
		throw new exception;

	// Local cache of verts
	std::vector<v3f> verts;

	// loop through .obj file for relevant data
	while (!file.eof())
	{
		// dirty assumption that no line has >128 characters
		char line[128];
		file.getline(line, 128);

		char junk;
		strstream s;
		s << line;

		// extract VERT values from obj then add to local cache
		if (line[0] == 'v')
		{
			v3f v;
			s >> junk >> v.x >> v.y >> v.z;
			verts.push_back(v);
		}


		// build TRIS from cached VERT values
		if (line[0] == 'f')
		{
			int f[3];
			s >> junk >> f[0] >> f[1] >> f[2];
			tris.push_back({ verts[f[0] - 1], verts[f[1] - 1], verts[f[2] - 1] });
		}
	}
}
