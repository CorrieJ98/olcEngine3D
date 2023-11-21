#include "olcConsoleGameEngine.h"
#include <fstream>
#include <strstream>
#include <algorithm>
#include <iostream>

using namespace std;

struct vec3d {
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;
	float w = 1.0f;
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
			// dirty assumption that no line has >128 characters
			char line[128];
			file.getline(line, 128);

			char junk;
			strstream s;
			s << line;



			// extract VERT values from obj then add to local cache
			if (line[0] == 'v')
			{
				vec3d v;
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
	// ::::: VARIABLE INITIALISATION :::::
	mesh meshObject;
	matrix4x4 projection_matrix;
	vec3d camera = {0,0,0};
	float cameraXSpeed = 8.0f;
	float cameraYSpeed = .0f;
	vec3d lookdir;
	float camYaw;
	float camZOffset = 6.0f;
	float rotAngle;
	const float PI = 3.141592;
	string objects[4] = { "axis.obj","mountains.obj","spaceship.obj","teapot.obj" };



	// ::::: MATRIX MATH :::::
	matrix4x4 Matrix_MakeIdentity() {
		matrix4x4 id;
		id.m[0][0] = 1.0f;
		id.m[1][1] = 1.0f;
		id.m[2][2] = 1.0f;
		id.m[3][3] = 1.0f;
		return id;
	}

	matrix4x4 Matrix_PointAt(vec3d &pos, vec3d &target, vec3d &up){
		// Calculate new forward (x) direction
		vec3d newFwd = Vector_Subtract(target, pos);
		newFwd = Vector_Normalise(newFwd);

		// Calculate new up (y) direction
		vec3d a = Vector_Multiply(newFwd, Vector_DotProduct(up, newFwd));
		vec3d newUp = Vector_Subtract(up, a);
		newUp = Vector_Normalise(newUp);
		
		// Calculate new right(z) direction
		vec3d newRight = Vector_CrossProduct(newUp, newFwd);

		// Construct Dimension and Translation Matrix
		matrix4x4 matDT;
		matDT.m[0][0] = newRight.x;		matDT.m[0][2] = newRight.z;
		matDT.m[1][0] = newUp.x;		matDT.m[1][2] = newUp.z;
		matDT.m[2][0] = newFwd.x;		matDT.m[2][2] = newFwd.z;	
		matDT.m[3][0] = pos.x;			matDT.m[3][2] = pos.z;
		matDT.m[0][1] = newRight.y;		matDT.m[0][3] = 0.0f;
		matDT.m[1][1] = newUp.y;		matDT.m[1][3] = 0.0f;
		matDT.m[2][1] = newFwd.y;		matDT.m[2][3] = 0.0f;
		matDT.m[3][1] = pos.y;			matDT.m[3][3] = 1.0f;
		return matDT;
	}


	matrix4x4 Matrix_QuickInvert(matrix4x4 &m) // Only for Rotation/Translation Matrices
	{
		matrix4x4 matrix;
		matrix.m[0][0] = m.m[0][0]; matrix.m[0][1] = m.m[1][0]; matrix.m[0][2] = m.m[2][0]; matrix.m[0][3] = 0.0f;
		matrix.m[1][0] = m.m[0][1]; matrix.m[1][1] = m.m[1][1]; matrix.m[1][2] = m.m[2][1]; matrix.m[1][3] = 0.0f;
		matrix.m[2][0] = m.m[0][2]; matrix.m[2][1] = m.m[1][2]; matrix.m[2][2] = m.m[2][2]; matrix.m[2][3] = 0.0f;
		matrix.m[3][0] = -(m.m[3][0] * matrix.m[0][0] + m.m[3][1] * matrix.m[1][0] + m.m[3][2] * matrix.m[2][0]);
		matrix.m[3][1] = -(m.m[3][0] * matrix.m[0][1] + m.m[3][1] * matrix.m[1][1] + m.m[3][2] * matrix.m[2][1]);
		matrix.m[3][2] = -(m.m[3][0] * matrix.m[0][2] + m.m[3][1] * matrix.m[1][2] + m.m[3][2] * matrix.m[2][2]);
		matrix.m[3][3] = 1.0f;
		return matrix;
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
		return matRotZ;
	}

	matrix4x4 Matrix_RotAxisY(float angleRad) {
		matrix4x4 matRotY;
		matRotY.m[0][0] = cosf(angleRad);
		matRotY.m[0][2] = sinf(angleRad);
		matRotY.m[2][0] = -sinf(angleRad);
		matRotY.m[1][1] = 1.0f;
		matRotY.m[2][2] = cosf(angleRad);
		matRotY.m[3][3] = 1.0f;
		return matRotY;
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

	// ::::: VECTOR MATH :::::
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
		meshObject.LoadFromObjectFile(objects[0]);

		// Projection Matrix
		projection_matrix = Matrix_MakeProjection(90.0f, (float)ScreenHeight() / (float)ScreenWidth(), 0.1f, 1000.0f);
		return true;
	}

	bool OnUserUpdate(float elapsedTime) override
	{

		vec3d vForward = Vector_Multiply(lookdir, 8.0f * elapsedTime);
		// ::::: KEYBINDS :::::
		if (GetKey(L'E').bHeld)	// Up
			camera.y += 8.0f * elapsedTime;
		
		if (GetKey(L'Q').bHeld)	// Down
			camera.y -= 8.0f * elapsedTime;

		if (GetKey(L'A').bHeld)	// Left
			camera.x += 8.0f * elapsedTime;

		if (GetKey(L'D').bHeld)	// Right
			camera.x -= 8.0f * elapsedTime;

		if (GetKey(L'W').bHeld)	// Forward
			camera = Vector_Add(camera, vForward);

		if (GetKey(L'S').bHeld) // Back
			camera = Vector_Subtract(camera, vForward);



		// Clear screen
		Fill(0, 0, ScreenWidth(), ScreenHeight(), PIXEL_SOLID, FG_BLACK);

		vector<triangle> vecTrianglesToRaster;

		// ::::: ROTATION MATRIX :::::
		matrix4x4 matRotZ, matRotX;
		// rotAngle += 1.0f * elapsedTime;

		matRotZ = Matrix_RotAxisZ(rotAngle * 0.5f);
		matRotX = Matrix_RotAxisX(rotAngle);

		matrix4x4 matTranslation;
		matTranslation = Matrix_MakeTranslate(0.0f, 0.0f, camZOffset);

		matrix4x4 matWorld;
		matWorld = Matrix_MakeIdentity();
		matWorld = Matrix_MultiplyMatrix(matRotZ, matRotX);
		matWorld = Matrix_MultiplyMatrix(matWorld, matTranslation);

		lookdir = { 0.0f,0.0f,1.0f };
		vec3d up = { 0.0f,1.0f,0.0f };
		vec3d target = Vector_Add(camera, lookdir);

		matrix4x4 camMatrix = Matrix_PointAt(camera, target, up);

		matrix4x4 matView = Matrix_QuickInvert(camMatrix);

		// Draw Triangles
		for (auto tri : meshObject.tris)
		{
			triangle triProjected, triTransformed, triViewed;

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

			// Raycast from tri to cam
			vec3d camRaycast = Vector_Subtract(triTransformed.p[0], camera);


			// ::::: CULLING :::::
			if (Vector_DotProduct(normal,camRaycast) < 0.0f) {

				// Illumination
				vec3d light_direction = { 0.0f, 1.0f,-1.0f};
				light_direction = Vector_Normalise(light_direction);

				// How aligned are the light direction and triangle normal?
				float dp = max(0.1f, Vector_DotProduct(light_direction, normal));

				// Get colour as required using dot product
				CHAR_INFO col = GetColour(dp);
				triTransformed.colour = col.Attributes;
				triTransformed.symbol = col.Char.UnicodeChar;

				// Convert world space into view space
				triViewed.p[0] = Matrix_MultiplyVector(matView, triTransformed.p[0]);
				triViewed.p[1] = Matrix_MultiplyVector(matView, triTransformed.p[1]);
				triViewed.p[2] = Matrix_MultiplyVector(matView, triTransformed.p[2]);

				// Project triangles 3D --> 2D
				triProjected.p[0] = Matrix_MultiplyVector(projection_matrix, triViewed.p[0]);
				triProjected.p[1] = Matrix_MultiplyVector(projection_matrix, triViewed.p[1]);
				triProjected.p[2] = Matrix_MultiplyVector(projection_matrix, triViewed.p[2]);
				triProjected.colour = triTransformed.colour;
				triProjected.symbol = triTransformed.symbol;

				// Scale into view
				triProjected.p[0] = Vector_Divide(triProjected.p[0], triProjected.p[0].w);
				triProjected.p[1] = Vector_Divide(triProjected.p[1], triProjected.p[1].w);
				triProjected.p[2] = Vector_Divide(triProjected.p[2], triProjected.p[2].w);

				// X/Y are inverted so put them back
				triProjected.p[0].x *= -1.0f;
				triProjected.p[1].x *= -1.0f;
				triProjected.p[2].x *= -1.0f;
				triProjected.p[0].y *= -1.0f;
				triProjected.p[1].y *= -1.0f;
				triProjected.p[2].y *= -1.0f;
				
				// Offset into screen space
				vec3d vOffsetView = { 1,1,0};
				triProjected.p[0] = Vector_Add(triProjected.p[0], vOffsetView);
				triProjected.p[1] = Vector_Add(triProjected.p[1], vOffsetView);
				triProjected.p[2] = Vector_Add(triProjected.p[2], vOffsetView);
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
};

int main()
{
	olcEngine3D demo;
	if (demo.ConstructConsole(256, 240, 4, 4))
		demo.Start();
    return 0;
}