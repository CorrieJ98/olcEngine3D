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

struct matrix4x4 {
	float m[4][4] = { 0 };
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
	mesh meshTetrahedron;
	matrix4x4 projection_matrix;
	vec3d camera;

	float rotAngle;

	void MultiplyMatrices(vec3d &in, vec3d &out, matrix4x4 &mat) {
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
			// SOUTH IS THE CLOSEST FACE PERPENDICULAR TO THE SCREEN
			// FACES ORIENTED RELATIVE TO THEIR NORMALS
			// TRIS (triangles ie 3 Vertices) ARE TAKEN CLOCKWISE FROM LOWER LEFT QUADRANT

			// SOUTH FACE
				{ 0.0f, 0.0f, 0.0f,    0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 0.0f },
				{ 0.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f,    1.0f, 0.0f, 0.0f },

			// EAST FACE
				{ 1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f },
				{ 1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 0.0f, 1.0f },

			// NORTH FACE
				{ 1.0f, 0.0f, 1.0f,    1.0f, 1.0f, 1.0f,    0.0f, 1.0f, 1.0f },
				{ 1.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 0.0f, 1.0f },

			// WEST FACE
				{ 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 1.0f, 0.0f },
				{ 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 0.0f,    0.0f, 0.0f, 0.0f },

			// TOP FACE
				{ 0.0f, 1.0f, 0.0f,    0.0f, 1.0f, 1.0f,    1.0f, 1.0f, 1.0f },
				{ 0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 1.0f, 0.0f },

			// BOTTOM FACE
				{ 1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f },
				{ 1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f,    1.0f, 0.0f, 0.0f },

		};
		meshTetrahedron.tris = {
			// VERTS
			// 1(1.0f, -1.0f, -1.0f)	2(-1.0f,-1.0f,1.0f)		3(1.0f,1.0f,1.0f)	4(-1.0f, 1.0f, -1.0f)

			// FACE A Verts 1-2-3
				{1.0f, -1.0f, -1.0f,	-1.0f,-1.0f,1.0f,		1.0f,1.0f,1.0f},

			// FACE B Verts 1-4-2
				{1.0f, -1.0f, -1.0f,   -1.0f, 1.0f, -1.0f,		-1.0f,-1.0f,1.0f},

			// FACE C Verts 4-3-2
				{-1.0f, 1.0f, -1.0f,    1.0f,1.0f,1.0f,    -1.0f,-1.0f,1.0f},

			// FACE D Verts 1-4-3
				{1.0f, -1.0f, -1.0f,    -1.0f, 1.0f, -1.0f,    1.0f,1.0f,1.0f},
		};

		// Projection Matrix
		float zNear = 0.1f;
		float zFar = 1000.0f;
		float fov = 90.0f;
		float aspect_ratio = (float)ScreenHeight() / (float)ScreenWidth();
		float fov_radians = 1.0f / tanf(fov * 0.5f / 180.0f * 3.14159);

		projection_matrix.m[0][0] = aspect_ratio * fov_radians;
		projection_matrix.m[1][1] = fov_radians;
		projection_matrix.m[2][2] = zFar / (zFar - zNear);
		projection_matrix.m[3][2] = (-zFar * zNear) / (zFar - zNear);
		projection_matrix.m[2][3] = 1.0f;
		projection_matrix.m[3][3] = 0.0f;

		return true;
	}

	bool OnUserUpdate(float elapsedTime) override
	{
		Fill(0, 0, ScreenWidth(), ScreenHeight(), PIXEL_SOLID, FG_BLACK);

		matrix4x4 matRotZ, matRotX;
		rotAngle += 1.0f * elapsedTime;

		// Z-Axis Rotation Matrix 
		matRotZ.m[0][0] = cosf(rotAngle);
		matRotZ.m[0][1] = sinf(rotAngle);
		matRotZ.m[1][0] = -sinf(rotAngle);
		matRotZ.m[1][1] = cosf(rotAngle);
		matRotZ.m[2][2] = 1;
		matRotZ.m[3][3] = 1;



		// X-Axis Rotation Matrix
		matRotX.m[0][0] = 1;
		matRotX.m[1][1] = cosf(rotAngle * 0.5f);
		matRotX.m[1][2] = sinf(rotAngle * 0.5f);
		matRotX.m[2][1] = -sinf(rotAngle * 0.5f);
		matRotX.m[2][2] = cosf(rotAngle * 0.5f);
		matRotX.m[3][3] = 1;



		// Draw Triangles
		for (auto tri : meshCube.tris)
		{
			triangle triProjected, triTranslated, triRotatedZ, triRotatedZX;

			// Rotate on Z-Axis
			MultiplyMatrices(tri.p[0], triRotatedZ.p[0], matRotZ);
			MultiplyMatrices(tri.p[1], triRotatedZ.p[1], matRotZ);
			MultiplyMatrices(tri.p[2], triRotatedZ.p[2], matRotZ);

			// Rotate on X-Axis
			MultiplyMatrices(triRotatedZ.p[0], triRotatedZX.p[0], matRotX);
			MultiplyMatrices(triRotatedZ.p[1], triRotatedZX.p[1], matRotX);
			MultiplyMatrices(triRotatedZ.p[2], triRotatedZX.p[2], matRotX);

			// Offset on Z-Axis as to be inside view
			triTranslated = triRotatedZX;
			triTranslated.p[0].z = triRotatedZX.p[0].z + 3.0f;
			triTranslated.p[1].z = triRotatedZX.p[1].z + 3.0f;
			triTranslated.p[2].z = triRotatedZX.p[2].z + 3.0f;



			// Get Normals
			vec3d normal, line1, line2;
			line1.x = triTranslated.p[1].x - triTranslated.p[0].x;
			line1.y = triTranslated.p[1].y - triTranslated.p[0].y;
			line1.z = triTranslated.p[1].z - triTranslated.p[0].z;

			line2.x = triTranslated.p[2].x - triTranslated.p[0].x;
			line2.y = triTranslated.p[2].y - triTranslated.p[0].y;
			line2.z = triTranslated.p[2].z - triTranslated.p[0].z;

			normal.x = line1.y * line2.z - line1.z * line2.y;
			normal.y = line1.z * line2.x - line1.x * line2.z;
			normal.z = line1.x * line2.y - line1.y * line2.x;

			// Normalize the normals
			float pythagResult = sqrt(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);
			normal.x /= pythagResult; normal.y /= pythagResult; normal.z /= pythagResult;

			// Project triangles from 3D onto 2D plane using projection matrix (ie screen)
			//if (normal.z < 0) primitive culling OBSOLETE
			if(normal.x * (triTranslated.p[0].x - camera.x) +
			   normal.y * (triTranslated.p[0].y - camera.y) +
			   normal.z * (triTranslated.p[0].z - camera.z) < 0){

				MultiplyMatrices(triTranslated.p[0], triProjected.p[0], projection_matrix);
				MultiplyMatrices(triTranslated.p[1], triProjected.p[1], projection_matrix);
				MultiplyMatrices(triTranslated.p[2], triProjected.p[2], projection_matrix);

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
			}


			// Rasterize triangle
			DrawTriangle(triProjected.p[0].x, triProjected.p[0].y,
				triProjected.p[1].x, triProjected.p[1].y,
				triProjected.p[2].x, triProjected.p[2].y,
				PIXEL_SOLID, FG_WHITE);

		}

		return true;
	}

};




int main()
{
	olcEngine3D demo;
	if (demo.ConstructConsole(256, 240, 4, 4))
		demo.Start();
    return 0;
}
