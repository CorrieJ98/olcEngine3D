#include "olcConsoleGameEngine.h"
#include <fstream>
#include <strstream>
#include <algorithm>
#include <iostream>

// https://youtu.be/HXSuNxpCzdM?t=409

using namespace std;

struct vec3d {
	float x, y, z, w;
};

struct triangle {
	vec3d p[3];

	wchar_t symbol;
	short colour;
};

struct mesh {

	vector<triangle> tris;

	bool LoadFromObjectFile(string sFilename)
	{
		ifstream file(sFilename);
		if (!file.is_open())
			return false;

		// Local cache of verts
		vector<vec3d> verts;

		// loop through .obj file for relevant data
		while (!file.eof())
		{
			// assuming no line has >128 chars
			char line[128];
			file.getline(line, 128);

			char junk;
			strstream s;
			s << line;



			// add vert values to cache
			if (line[0] == 'v')
			{
				vec3d v;
				s >> junk >> v.x >> v.y >> v.z;
				verts.push_back(v);
			}


			// build tris
			if (line[0] == 'f')
			{
				int f[3];
				s >> junk >> f[0] >> f[1] >> f[2];
				tris.push_back({ verts[f[0] - 1], verts[f[1] - 1], verts[f[2] - 1] });
			}
		}

		return true;
	}
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
	mesh meshObject;
	matrix4x4 projection_matrix;
	vec3d camera;
	float rotAngle;
	const float PI = 3.141592;


	// ::::: MATRIX MATH :::::
	// :: Matrices
	matrix4x4 Matrix_MakeIdentity() {
		matrix4x4 id;
		id.m[0][0] = 1.0f;
		id.m[1][1] = 1.0f;
		id.m[2][2] = 1.0f;
		id.m[3][3] = 1.0f;
		return id;
	}

	matrix4x4 Matrix_RotAxisX(float angleRad) {
		matrix4x4 matRotX;
		matRotX.m[0][0] = 1.0f;
		matRotX.m[1][1] = cosf(angleRad);
		matRotX.m[1][2] = sinf(angleRad);
		matRotX.m[2][1] = -sinf(angleRad);
		matRotX.m[2][2] = cosf(angleRad);
		matRotX.m[3][3] = 1.0f;
		return matRotX;
	}

	matrix4x4 Matrix_RotAxisZ(float angleRad) {
		matrix4x4 matRotZ;
		matRotZ.m[0][0] = cosf(angleRad);
		matRotZ.m[0][1] = sinf(angleRad);
		matRotZ.m[1][0] = -sinf(angleRad);
		matRotZ.m[1][1] = cosf(angleRad);
		matRotZ.m[2][2] = 1.0f;
		matRotZ.m[3][3] = 1.0f;
	}

	matrix4x4 Matrix_RotAxisY(float angleRad) {
		matrix4x4 matRotY;
		matRotY.m[0][0] = cosf(angleRad);
		matRotY.m[0][2] = sinf(angleRad);
		matRotY.m[2][0] = -sinf(angleRad);
		matRotY.m[1][1] = 1.0f;
		matRotY.m[2][2] = cosf(angleRad);
		matRotY.m[3][3] = 1.0f;
	}

	matrix4x4 Matrix_MakeTranslate(float x, float y, float z) {
		matrix4x4 matT;
		matT.m[0][0] = 1.0f;
		matT.m[1][1] = 1.0f;
		matT.m[2][2] = 1.0f;
		matT.m[3][3] = 1.0f;
		matT.m[3][0] = x;
		matT.m[3][1] = y;
		matT.m[3][2] = z;
		return matT;
	}

	matrix4x4 Matrix_MakeProjection(float fov, float aspect_ratio, float zNear, float zFar){
		float fovRad = 1.0f / tanf(fov * 0.5f / 180.0f * PI);
		matrix4x4 matProj;
		matProj.m[0][0] = aspect_ratio * fovRad;
		matProj.m[1][1] = fovRad;
		matProj.m[2][2] = zFar / (zFar - zNear);
		matProj.m[3][2] = (-zFar * zNear) / (zFar - zNear);
		matProj.m[2][3] = 1.0f;
		matProj.m[3][3] = 0.0f;
		return matProj;
	}

	matrix4x4 Matrix_MultiplyMatrix(matrix4x4& m1, matrix4x4& m2) {
		matrix4x4 mout;
		for (int c = 0; c < 4; c++)
			for (int r = 0; r < 4; r++)
				mout.m[r][c] = m1.m[r][0] * m2.m[0][c] + m1.m[r][1] * m2.m[1][c] + m1.m[r][2] * m2.m[2][c] + m1.m[r][3] * m2.m[3][c];
		return mout;
	}

	vec3d Matrix_MultiplyVector(matrix4x4& m, vec3d& vin) {
		vec3d vout;
		vout.x = vin.x * m.m[0][0] + vin.y * m.m[1][0] + vin.z * m.m[2][0] + vin.w * m.m[3][0];
		vout.y = vin.x * m.m[0][1] + vin.y * m.m[1][1] + vin.z * m.m[2][1] + vin.w * m.m[3][1];
		vout.z = vin.x * m.m[0][2] + vin.y * m.m[1][2] + vin.z * m.m[2][2] + vin.w * m.m[3][2];
		vout.w = vin.x * m.m[0][3] + vin.y * m.m[1][3] + vin.z * m.m[2][3] + vin.w * m.m[3][3];
		return vout;
	}

	// :: Vectors
	vec3d Vector_Add(vec3d& v1, vec3d& v2) {
		return { v1.x + v2.x, v1.y + v2.y, v1.z + v2.z };
	}

	vec3d Vector_Subtract(vec3d& v1, vec3d& v2) {
		return { v1.x - v2.x, v1.y - v2.y, v1.z - v2.z };
	}

	vec3d Vector_Multiply(vec3d& v1, float k) {
		return { v1.x * k, v1.y * k, v1.z * k };
	}

	vec3d Vector_Divide(vec3d& v1,float k) {
		return { v1.x / k, v1.y / k, v1.z / k };
	}

	float Vector_DotProduct(vec3d& v1, vec3d& v2) {
		return { v1.x * v2.x + v1.y * v2.y + v1.z * v2.z };
	}

	float Vector_Length(vec3d& vec) {
		return sqrtf(Vector_DotProduct(vec, vec));
	}

	vec3d Vector_Normalise(vec3d &vec) {
		float l = Vector_Length(vec);
		return { vec.x / l, vec.y / l, vec.z / l };
	}

	vec3d Vector_CrossProduct(vec3d& v1, vec3d& v2) {
		vec3d v;
		v.x = v1.y * v2.z - v1.z * v2.y;
		v.y = v1.z * v2.x - v1.x * v2.z;
		v.z = v1.x * v2.y - v1.y * v2.x;
		return v;
	}


	CHAR_INFO GetColour(float lum) {
		short bg_colour, fg_colour;
		wchar_t symbol;
		int pixel_bw = (int)(13.0f * lum);

		switch (pixel_bw) {
			case 0: bg_colour = BG_BLACK; fg_colour = FG_BLACK; symbol = PIXEL_SOLID; break;

			case 1: bg_colour = BG_BLACK; fg_colour = FG_DARK_GREY; symbol = PIXEL_QUARTER; break;
			case 2: bg_colour = BG_BLACK; fg_colour = FG_DARK_GREY; symbol = PIXEL_HALF; break;
			case 3: bg_colour = BG_BLACK; fg_colour = FG_DARK_GREY; symbol = PIXEL_THREEQUARTERS; break;
			case 4: bg_colour = BG_BLACK; fg_colour = FG_DARK_GREY; symbol = PIXEL_SOLID; break;

			case 5: bg_colour = BG_DARK_GREY; fg_colour = FG_GREY; symbol = PIXEL_QUARTER; break;
			case 6: bg_colour = BG_DARK_GREY; fg_colour = FG_GREY; symbol = PIXEL_HALF; break;
			case 7: bg_colour = BG_DARK_GREY; fg_colour = FG_GREY; symbol = PIXEL_THREEQUARTERS; break;
			case 8: bg_colour = BG_DARK_GREY; fg_colour = FG_GREY; symbol = PIXEL_SOLID; break;

			case 9: bg_colour = BG_GREY; fg_colour = FG_WHITE; symbol = PIXEL_QUARTER; break;
			case 10: bg_colour = BG_GREY; fg_colour = FG_WHITE; symbol = PIXEL_HALF; break;
			case 11: bg_colour = BG_GREY; fg_colour = FG_WHITE; symbol = PIXEL_THREEQUARTERS; break;
			case 12: bg_colour = BG_GREY; fg_colour = FG_WHITE; symbol = PIXEL_SOLID; break;
			default:
				bg_colour = BG_BLACK; fg_colour = FG_BLACK; symbol = PIXEL_SOLID;

		}

		CHAR_INFO col;
		col.Attributes = bg_colour | fg_colour;
		col.Char.UnicodeChar = symbol;

		return col;
	}


public:
	bool OnUserCreate() override
	{
		// .obj file import
		meshObject.LoadFromObjectFile("spaceship.obj");

		projection_matrix = Matrix_MakeProjection(90.0f, (float)ScreenHeight() / (float)ScreenWidth(), 0.1f, 1000.0f);
		return true;
	}

	bool OnUserUpdate(float elapsedTime) override
	{
		Fill(0, 0, ScreenWidth(), ScreenHeight(), PIXEL_SOLID, FG_BLACK);

		vector<triangle> vecTrianglesToRaster;

		// ::::: Rotation Matrices :::::
		matrix4x4 matRotZ, matRotX;
		rotAngle += 1.0f * elapsedTime;

		matRotZ = Matrix_RotAxisZ(rotAngle * 0.5f);
		matRotX = Matrix_RotAxisX(rotAngle);

		matrix4x4 matTrans;
		matTrans = Matrix_MakeTranslate(0.0f, 0.0f, 16.0f);

		matrix4x4 matWorld;
		matWorld = Matrix_MakeIdentity();
		matWorld = Matrix_MultiplyMatrix(matRotZ, matRotX);
		matWorld = Matrix_MultiplyMatrix(matWorld, matTrans);


		// Draw Triangles
		for (auto tri : meshObject.tris)
		{
			triangle triProjected, triTransformed;

			triTransformed.p[0] = Matrix_MultiplyVector(matWorld, tri.p[0]);
			triTransformed.p[1] = Matrix_MultiplyVector(matWorld, tri.p[1]);
			triTransformed.p[2] = Matrix_MultiplyVector(matWorld, tri.p[2]);
			
			// Calculate triangle Normal
			vec3d normal, line1, line2;

			// Get lines either side of triangle
			line1 = Vector_Subtract(triTransformed.p[1], triTransformed.p[0]);
			line2 = Vector_Subtract(triTransformed.p[2], triTransformed.p[0]);

			// Take Cross Product of line to get normal to tri surface
			normal = Vector_CrossProduct(line1, line2);

			// Normalise normals!
			normal = Vector_Normalise(normal);

			// Culling
			if (normal.x * (triTranslated.p[0].x - camera.x) +
				normal.y * (triTranslated.p[0].y - camera.y) +
				normal.z * (triTranslated.p[0].z - camera.z) < 0.0f) {

				// Illuminate the cube
				vec3d light_direction = { 0.0f,0.0f,-1.0f };

				float l = sqrt(light_direction.x * light_direction.x + light_direction.y * light_direction.y + light_direction.z * light_direction.z);
				light_direction.x /= l; light_direction.y /= l; light_direction.z /= l;

				float dp = normal.x * light_direction.x + normal.y * light_direction.y + normal.z * light_direction.z;

				// Get colour as required using dot product
				CHAR_INFO col = GetColour(dp);
				triTranslated.colour = col.Attributes;
				triTranslated.symbol = col.Char.UnicodeChar;

				// Project triangles 3D --> 2D
				MultiplyMatrices(triTranslated.p[0], triProjected.p[0], projection_matrix);
				MultiplyMatrices(triTranslated.p[1], triProjected.p[1], projection_matrix);
				MultiplyMatrices(triTranslated.p[2], triProjected.p[2], projection_matrix);
				triProjected.colour = triTranslated.colour;
				triProjected.symbol = triTranslated.symbol;

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

				vecTrianglesToRaster.push_back(triProjected);
			}
		}

		// Sort tris back to front
		sort(vecTrianglesToRaster.begin(), vecTrianglesToRaster.end(), [](triangle& t1, triangle& t2)
			{
				float z1 = (t1.p[0].z + t1.p[1].z + t1.p[2].z) / 3.0f;
				float z2 = (t2.p[0].z + t2.p[1].z + t2.p[2].z) / 3.0f;
				return z1 > z2;
			});

		for (auto& triProjected : vecTrianglesToRaster)
		{
			// Rasterize triangle
			FillTriangle(triProjected.p[0].x, triProjected.p[0].y,
				triProjected.p[1].x, triProjected.p[1].y,
				triProjected.p[2].x, triProjected.p[2].y,
				triProjected.symbol, triProjected.colour);


			// OBSOLETE Direct Draw
			/*DrawTriangle(triProjected.p[0].x, triProjected.p[0].y,
			triProjected.p[1].x, triProjected.p[1].y,
			triProjected.p[2].x, triProjected.p[2].y,
			PIXEL_SOLID, FG_BLACK);*/
		}


		return true;
	}

	float Q_rsqrt(float number) {
		long i;
		float x2, y;
		const float threehalfs = 1.5f;

		x2 = number * 0.5f;
		y = number;
		i = * (long *) &y;					// evil floating point bit hack
		i = 0x5f3759df - (i >> 1);			// what the fuck?
		y = *(float*) &i;
		y = y * (threehalfs - (x2 * y * y));	// 1st iteration
		//y = y * (threehalfs - (x2 * y * y));	// 2nd iteration, this can be removed
	}
};




int main()
{
	olcEngine3D demo;
	if (demo.ConstructConsole(256, 240, 4, 4))
		demo.Start();
    return 0;
}
