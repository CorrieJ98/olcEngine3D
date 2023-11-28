#include "olcConsoleGameEngine.h"
#include <fstream>
#include <strstream>
#include <algorithm>
#include <iostream>

using namespace std;

// TODO https://youtu.be/HXSuNxpCzdM?list=PLrOv9FMX8xJE8NgepZR1etrsU63fDDGxO&t=1783

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
	vec3d eye = {0,0,0};
	float cameraXSpeed = 8.0f;
	float cameraYSpeed = 8.0f;
	float camYaw = 0.0f;
	float camPitch = 0.0f;
	float camZOffset = 6.0f;
	vec3d lookdir;
	float rotAngle;
	const float PI = 3.141592653;
	string objects[4] = { "axis.obj","mountains.obj","spaceship.obj","teapot.obj" };


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
		projection_matrix = Matrix_MakeProjection(90.0f, (float)ScreenHeight() / (float)ScreenWidth(), 0.01f, 1000.0f);
		return true;
	}

	bool OnUserUpdate(float elapsedTime) override{
	
		vec3d vForward = MatVecMath::Vector_Multiply(lookdir, 8.0f * elapsedTime);
		// vec3d vForward = Vector_Multiply(lookdir, 8.0f * elapsedTime);
	
		// Camera Matrix
		vec3d up = { 0.0f,1.0f,0.0f };
		vec3d target = {0.0f,0.0f,1.0f};
		matrix4x4 yaw = Matrix_RotAxisY(camYaw);
		matrix4x4 pitch = Matrix_RotAxisX(camPitch);
		matrix4x4 matCamRotation = Matrix_MultiplyMatrix(yaw,pitch);
		lookdir = Matrix_MultiplyVector(matCamRotation, target);
		target = Vector_Add(eye, lookdir);
		matrix4x4 matCamera = Matrix_PointAt(eye, target, up);
		matrix4x4 matView = Matrix_QuickInvert(matCamera);

		// ::::: KEYBINDINGS :::::
		if (GetKey(L'E').bHeld)	// Up
			eye.y += 8.0f * elapsedTime;
		
		if (GetKey(L'Q').bHeld)	// Down
			eye.y -= 8.0f * elapsedTime;

		if (GetKey(L'A').bHeld)	// Left
			eye.x += 8.0f * elapsedTime;

		if (GetKey(L'D').bHeld)	// Right
			eye.x -= 8.0f * elapsedTime;

		if (GetKey(L'W').bHeld)	// Forward
			eye = Vector_Add(eye, vForward);

		if (GetKey(L'S').bHeld) // Back
			eye = Vector_Subtract(eye, vForward);

		if (GetKey(VK_LEFT).bHeld)	// Yaw Left
			camYaw -= 2.0f * elapsedTime;

		if (GetKey(VK_RIGHT).bHeld)	// Yaw Right
			camYaw += 2.0f * elapsedTime;

		if (GetKey(VK_UP).bHeld)	// Pitch Up
			camPitch += 2.0f * elapsedTime;

		if (GetKey(VK_DOWN).bHeld)	// Pitch Down
			camPitch -= 2.0f * elapsedTime;


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
			vec3d camRaycast = Vector_Subtract(triTransformed.p[0], eye);


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