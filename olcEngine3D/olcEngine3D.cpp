#include "olcConsoleGameEngine.h"

struct vec3d {
	float x, y, z;
};

struct triangle {
	vec3d p[3];
};

struct mesh {
	std::vector<triangle> tris;
};

struct mat4x4 {
	float m[4][4];
};



class olcEngine3D : public olcConsoleGameEngine
{
public:
	olcEngine3D() 
	{
		m_sAppName = L"3D Demo";
	}



private:
	mesh meshCube;
	mat4x4 matProjection;

	void MultiplyMatrixVector(vec3d &in, vec3d &out, mat4x4 &mat) {
		out.x = in.x * mat.m[0][0] + in.y * mat.m[1][0] + in.z * mat.m[2][0] + mat.m[3][0];
		out.y = in.x * mat.m[0][1] + in.y * mat.m[1][1] + in.z * mat.m[2][1] + mat.m[3][1];
		out.z = in.x * mat.m[0][2] + in.y * mat.m[1][2] + in.z * mat.m[2][2] + mat.m[3][2];
		float w = in.x* mat.m[0][3] + in.y * mat.m[1][3] + in.z * mat.m[2][3] + mat.m[3][3];

		if (w != 0) {
			out.x /= w; out.y /= w; out.z /= w;
		}
	}



public:
	bool OnUserCreate() override
	{
		meshCube.tris = {
		// NORTH
			{0.0f,0.0f,1.0f,	0.0f,1.0f,1.0f,		1.0f,1.0f,1.0f},
			{0.0f,0.0f,1.0f,	1.0f,1.0f,1.0f,		1.0f,0.0f,1.0f},

		// SOUTH
			{0.0f,0.0f,0.0f,	0.0f,1.0f,0.0f,		1.0f,1.0f,0.0f},
			{0.0f,0.0f,0.0f,	1.0f,1.0f,0.0f,		1.0f,1.0f,1.0f},

		// EAST
			{1.0f,0.0f,0.0f,	1.0f,1.0f,0.0f,		1.0f,1.0f,1.0f},
			{1.0f,0.0f,0.0f,	1.0f,1.0f,1.0f,		1.0f,0.0f,1.0f},

		// WEST
			{0.0f,0.0f,1.0f,	0.0f,1.0f,1.0f,		0.0f,1.0f,0.0f},
			{0.0f,0.0f,1.0f,	0.0f,1.0f,0.0f,		0.0f,0.0f,0.0f},

		// TOP
			{0.0f,1.0f,0.0f,	0.0f,1.0f,1.0f,		1.0f,1.0f,1.0f},
			{0.0f,1.0f,0.0f,	1.0f,1.0f,1.0f,		1.0f,1.0f,0.0f},

		// BOTTOM
			{0.0f,0.0f,0.0f,	0.0f,0.0f,1.0f,		1.0f,0.0f,1.0f},
			{0.0f,0.0f,0.0f,	1.0f,0.0f,1.0f,		1.0f,0.0f,0.0f},
		};

		// Projection Matrix
		float zNear = 0.1f;
		float zFar = 1000.0f;
		float fov = 90.0f;
		float aspect_ratio = (float)ScreenHeight() / (float)ScreenWidth();
		float fov_radians = 1.0f / tanf(fov * 0.5f / 180.0f * 3.141592653f);

		matProjection.m[0][0] = aspect_ratio * fov_radians;
		matProjection.m[1][1] = fov_radians;
		matProjection.m[2][2] = zFar / (zFar - zNear);
		matProjection.m[3][2] = (-zFar * zNear) / (zFar - zNear);
		matProjection.m[2][3] = 1.0f;
		matProjection.m[3][3] = 0.0f;

		return true;
	}

	bool OnUserUpdate(float elapsedTime) override
	{

		Fill(0, 0, ScreenWidth(), ScreenHeight(), PIXEL_SOLID, FG_BLACK);

		//Draw Triangles
		for (auto tri : meshCube.tris) {
			triangle triProjected;
			MultiplyMatrixVector(tri.p[0], triProjected.p[0], matProjection);
			MultiplyMatrixVector(tri.p[1], triProjected.p[1], matProjection);
			MultiplyMatrixVector(tri.p[2], triProjected.p[2], matProjection);

			// Scale into view
			triProjected.p[0].x += 1.0f; triProjected.p[0].y += 1.0f;
			triProjected.p[1].x += 1.0f; triProjected.p[1].y += 1.0f;
			triProjected.p[2].x += 1.0f; triProjected.p[2].y += 1.0f;

			triProjected.p[0].x *= 0.5f * (float)ScreenWidth();
			triProjected.p[0].y *= 0.5f * (float)ScreenHeight();
			triProjected.p[1].x *= 0.5f * (float)ScreenWidth();
			triProjected.p[1].y *= 0.5f * (float)ScreenHeight();
			triProjected.p[2].x *= 0.5f * (float)ScreenWidth();
			triProjected.p[2].y *= 0.5f * (float)ScreenHeight();

			DrawTriangle(
				triProjected.p[0].x, triProjected.p[0].y,
				triProjected.p[1].x, triProjected.p[1].y,
				triProjected.p[2].x, triProjected.p[2].y,
				PIXEL_SOLID, FG_WHITE);
		}

		return true;
	}
};


int main() {

	olcEngine3D demo;
	if (demo.ConstructConsole(256, 240, 4, 4))
		demo.Start();
	return 0;
}