#include <iostream>
#include <fstream>
#include <algorithm>
#include <strstream>
#include <string>
#include <vector>
#include "mvmath.h"
#include "olcConsoleGameEngine.h"


using namespace std;

MV_Math mv;

struct camera {
	v3f pos;
	float pitch, yaw, sensitivity;
};
struct mesh {

	vector<triangle> tris;

	bool LoadFromObjectFile(string sFilename)
	{
		ifstream file(sFilename);
		if (!file.is_open())
			return false;

		// Local cache of verts
		vector<v3f> verts;

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

		return true;
	}
};

class olcEngine3D : public olcConsoleGameEngine
{
public:
	olcEngine3D() 
	{
		m_sAppName = L"Tankers 3D";
	}

	// namespace mv

private:
	// ::::: VARIABLE INITIALISATION :::::
	mesh testMesh;
	mat4x4 projection_matrix;
	mat4x4 identity_matrix;
	camera eye;

	v3f lookdir;
	float camZOffset = 6.0f;
	const float PI = 3.141592653;
	string objects[4] = { "axis.obj","mountains.obj","spaceship.obj","teapot.obj" };

	float RadToDeg(float rad) {
		return rad *= 180 / PI;
	}
	float DegToRad(float deg) {
		return deg *= PI / 180;
	}
	CHAR_INFO GetColour(float lum)
	{
		short bg_col, fg_col;
		wchar_t sym;
		int pixel_bw = (int)(13.0f * lum);
		switch (pixel_bw)
		{
		case 0: bg_col = BG_BLACK; fg_col = FG_BLACK; sym = PIXEL_SOLID; break;

		case 1: bg_col = BG_BLACK; fg_col = FG_DARK_GREY; sym = PIXEL_QUARTER; break;
		case 2: bg_col = BG_BLACK; fg_col = FG_DARK_GREY; sym = PIXEL_HALF; break;
		case 3: bg_col = BG_BLACK; fg_col = FG_DARK_GREY; sym = PIXEL_THREEQUARTERS; break;
		case 4: bg_col = BG_BLACK; fg_col = FG_DARK_GREY; sym = PIXEL_SOLID; break;

		case 5: bg_col = BG_DARK_GREY; fg_col = FG_GREY; sym = PIXEL_QUARTER; break;
		case 6: bg_col = BG_DARK_GREY; fg_col = FG_GREY; sym = PIXEL_HALF; break;
		case 7: bg_col = BG_DARK_GREY; fg_col = FG_GREY; sym = PIXEL_THREEQUARTERS; break;
		case 8: bg_col = BG_DARK_GREY; fg_col = FG_GREY; sym = PIXEL_SOLID; break;

		case 9:  bg_col = BG_GREY; fg_col = FG_WHITE; sym = PIXEL_QUARTER; break;
		case 10: bg_col = BG_GREY; fg_col = FG_WHITE; sym = PIXEL_HALF; break;
		case 11: bg_col = BG_GREY; fg_col = FG_WHITE; sym = PIXEL_THREEQUARTERS; break;
		case 12: bg_col = BG_GREY; fg_col = FG_WHITE; sym = PIXEL_SOLID; break;
		default:
			bg_col = BG_BLACK; fg_col = FG_BLACK; sym = PIXEL_SOLID;
		}

		CHAR_INFO c;
		c.Attributes = bg_col | fg_col;
		c.Char.UnicodeChar = sym;
		return c;
	}

	float mouseX, mouseY;
	void CursorManager(float mXin, float mYin, float mXout, float mYout, float sens) {

		// GetMouse position values, subtract from the centre point of screen

		v3f halfres = {
			// take int x and bitshift 1 place to the right for x/2
			ScreenWidth() >> 1,
			ScreenHeight() >> 1,
		};

		mXout = mXin - halfres.x * (sens * 0.01f);
		mYout = mYin - halfres.y * (sens * 0.01f);
	};


public:
	bool OnUserCreate() override
	{
		// .obj file import
		testMesh.LoadFromObjectFile(objects[1]);

		// Projection Matrix
		projection_matrix = mv.Matrix_MakeProjection(90.0f, (float)ScreenHeight() / (float)ScreenWidth(), 0.01f, 1000.0f);
		return true;
	}

	bool OnUserUpdate(float elapsedTime) override
	{
		v3f up = { 0.0f,1.0f,0.0f };
		v3f target = { 0.0f,0.0f,1.0f };

		mat4x4 ninetyY = mv.Matrix_RotAxisY(0.5f * PI);
		mat4x4 ninetyX = mv.Matrix_RotAxisX(1.5f * PI);
		v3f r = mv.Matrix_MultiplyVector(ninetyY, lookdir);
		v3f u = mv.Matrix_MultiplyVector(ninetyX, lookdir);
		v3f vForward = mv.Vector_Multiply(lookdir, 8.0f * elapsedTime);
		v3f vShoulderRail = mv.Vector_Multiply(r, 8.0f * elapsedTime);
		v3f vBackRail = mv.Vector_Multiply(u, 8.0f * elapsedTime);

		mat4x4 y = mv.Matrix_RotAxisY(eye.yaw);
		mat4x4 p = mv.Matrix_RotAxisX(eye.pitch);

		mat4x4 camMatrixRot = mv.Matrix_RotAxisY(eye.yaw);
		lookdir = mv.Matrix_MultiplyVector(camMatrixRot, target);
		target = mv.Vector_Add(eye.pos, lookdir);
		mat4x4 camMatrix = mv.Matrix_PointAt(eye.pos, target, up);
		mat4x4 matView = mv.Matrix_QuickInvert(camMatrix);

		// ::::: ROTATION MATRIX :::::
		mat4x4 matRotY, matRotX;
		matRotY = mv.Matrix_RotAxisY(eye.yaw);
		matRotX = mv.Matrix_RotAxisX(eye.pitch);

		mat4x4 matTranslation;
		matTranslation = mv.Matrix_MakeTranslation(0.0f, 0.0f, camZOffset);

		mat4x4 matWorld;
		mv.Matrix_MakeIdentity(matWorld);
		
		/*matWorld = mv.Matrix_MakeIdentity();
		matWorld = mv.Matrix_MultiplyMatrix(matRotY, matRotX);
		matWorld = mv.Matrix_MultiplyMatrix(matWorld, matTranslation);*/



		// ::::: KEYBINDINGS :::::

		// Movement
		if (GetKey(L'E').bHeld)	// Up
			eye.pos.y += 8.0f * elapsedTime;

		if (GetKey(L'Q').bHeld)	// Down
			eye.pos.y -= 8.0f * elapsedTime;

		if (GetKey(L'A').bHeld)	// Left
			eye.pos = mv.Vector_Subtract(eye.pos, vShoulderRail);

		if (GetKey(L'D').bHeld)	// Right
			eye.pos = mv.Vector_Add(eye.pos, vShoulderRail);

		if (GetKey(L'W').bHeld)	// Forward
			eye.pos = mv.Vector_Add(eye.pos, vForward);

		if (GetKey(L'S').bHeld) // Back
			eye.pos = mv.Vector_Subtract(eye.pos, vForward);


		// Camera Keyboard
		if (GetKey(VK_LEFT).bHeld)	// Yaw Left
			eye.yaw -= 2.0f * elapsedTime;

		if (GetKey(VK_RIGHT).bHeld)	// Yaw Right
			eye.yaw += 2.0f * elapsedTime;

		if (GetKey(VK_UP).bHeld)	// Pitch Up
			eye.pitch -= 2.0f * elapsedTime;

		if (GetKey(VK_DOWN).bHeld)	// Pitch Down
			eye.pitch += 2.0f * elapsedTime;

		//// Camera Mouse
		//CursorManager(GetMouseX(), GetMouseY(), mouseX, mouseY, 1.0f);
		//if (mouseX != (ScreenWidth() >> 1)) {
		//	eye.yaw += mouseX;
		//}

		//if (mouseY != (ScreenHeight() >> 1)) {
		//	eye.pitch += mouseY;
		//};


		// Clear screen
		Fill(0, 0, ScreenWidth(), ScreenHeight(), PIXEL_SOLID, FG_BLACK);


		vector<triangle> vecTrianglesToRaster;


		// Draw Triangles
		for (auto tri : testMesh.tris)
		{
			triangle triProjected, triTransformed, triViewed;

			// World Matrix Transform
			triTransformed.p[0] = mv.Matrix_MultiplyVector(matWorld, tri.p[0]);
			triTransformed.p[1] = mv.Matrix_MultiplyVector(matWorld, tri.p[1]);
			triTransformed.p[2] = mv.Matrix_MultiplyVector(matWorld, tri.p[2]);

			// Calculate triangle Normal
			v3f normal, line1, line2;

			// Get lines either side of triangle
			line1 = mv.Vector_Subtract(triTransformed.p[1], triTransformed.p[0]);
			line2 = mv.Vector_Subtract(triTransformed.p[2], triTransformed.p[0]);

			// Take cross product of lines to get normal to triangle surface
			normal = mv.Vector_CrossProduct(line1, line2);

			// You normally need to normalise a normal!
			normal = mv.Vector_Normalise(normal);

			// Get Ray from triangle to camera
			v3f vCameraRay = mv.Vector_Subtract(triTransformed.p[0], eye.pos);

			// If ray is aligned with normal, then triangle is visible
			if (mv.Vector_DotProduct(normal, vCameraRay) < 0.0f)
			{
				// Illumination
				v3f light_direction = { 1.0f, 1.5f, 2.0f };
				light_direction = mv.Vector_Normalise(light_direction);

				// How "aligned" are light direction and triangle surface normal?
				float dp = max(0.1f, mv.Vector_DotProduct(light_direction, normal));

				// Choose console colours as required (much easier with RGB)
				CHAR_INFO c = GetColour(dp);
				triTransformed.col = c.Attributes;
				triTransformed.sym = c.Char.UnicodeChar;

				// Convert World Space --> View Space
				triViewed.p[0] = mv.Matrix_MultiplyVector(matView, triTransformed.p[0]);
				triViewed.p[1] = mv.Matrix_MultiplyVector(matView, triTransformed.p[1]);
				triViewed.p[2] = mv.Matrix_MultiplyVector(matView, triTransformed.p[2]);
				triViewed.sym = triTransformed.sym;
				triViewed.col = triTransformed.col;

				// Clip Viewed Triangle against near plane, this could form two additional
				// additional triangles. 
				int nClippedTriangles = 0;
				triangle clipped[2];
				nClippedTriangles = mv.Triangle_ClipAgainstPlane({ 0.0f, 0.0f, 0.1f }, { 0.0f, 0.0f, 1.0f }, triViewed, clipped[0], clipped[1]);

				// We may end up with multiple triangles form the clip, so project as
				// required
				for (int n = 0; n < nClippedTriangles; n++)
				{
					// Project triangles from 3D --> 2D
					triProjected.p[0] = mv.Matrix_MultiplyVector(projection_matrix, clipped[n].p[0]);
					triProjected.p[1] = mv.Matrix_MultiplyVector(projection_matrix, clipped[n].p[1]);
					triProjected.p[2] = mv.Matrix_MultiplyVector(projection_matrix, clipped[n].p[2]);
					triProjected.col = clipped[n].col;
					triProjected.sym = clipped[n].sym;

					// Scale into view, we moved the normalising into cartesian space
					// out of the matrix.vector function from the previous videos, so
					// do this manually
					triProjected.p[0] = mv.Vector_Divide(triProjected.p[0], triProjected.p[0].w);
					triProjected.p[1] = mv.Vector_Divide(triProjected.p[1], triProjected.p[1].w);
					triProjected.p[2] = mv.Vector_Divide(triProjected.p[2], triProjected.p[2].w);

					// X/Y are inverted so put them back
					triProjected.p[0].x *= -1.0f;
					triProjected.p[1].x *= -1.0f;
					triProjected.p[2].x *= -1.0f;
					triProjected.p[0].y *= -1.0f;
					triProjected.p[1].y *= -1.0f;
					triProjected.p[2].y *= -1.0f;

					// Offset verts into visible normalised space
					v3f vOffsetView = { 1,1,0 };
					triProjected.p[0] = mv.Vector_Add(triProjected.p[0], vOffsetView);
					triProjected.p[1] = mv.Vector_Add(triProjected.p[1], vOffsetView);
					triProjected.p[2] = mv.Vector_Add(triProjected.p[2], vOffsetView);
					triProjected.p[0].x *= 0.5f * (float)ScreenWidth();
					triProjected.p[0].y *= 0.5f * (float)ScreenHeight();
					triProjected.p[1].x *= 0.5f * (float)ScreenWidth();
					triProjected.p[1].y *= 0.5f * (float)ScreenHeight();
					triProjected.p[2].x *= 0.5f * (float)ScreenWidth();
					triProjected.p[2].y *= 0.5f * (float)ScreenHeight();

					// Store triangle for sorting
					vecTrianglesToRaster.push_back(triProjected);
				}
			}
		}

		// Sort tris back to front
		sort(vecTrianglesToRaster.begin(), vecTrianglesToRaster.end(), [](triangle& t1, triangle& t2)
			{
				float z1 = (t1.p[0].z + t1.p[1].z + t1.p[2].z) / 3.0f;
				float z2 = (t2.p[0].z + t2.p[1].z + t2.p[2].z) / 3.0f;
				return z1 > z2;
			});

		for (auto& triToRaster : vecTrianglesToRaster)
		{
			// Clip triangles against all four screen edges, this could yield
			// a bunch of triangles, so create a queue that we traverse to 
			//  ensure we only test new triangles generated against planes
			triangle clipped[2];
			list<triangle> listTriangles;

			// Add initial triangle
			listTriangles.push_back(triToRaster);
			int nNewTriangles = 1;

			for (int p = 0; p < 4; p++)
			{
				int nTrisToAdd = 0;
				while (nNewTriangles > 0)
				{
					// Take triangle from front of queue
					triangle test = listTriangles.front();
					listTriangles.pop_front();
					nNewTriangles--;

					// Clip it against a plane. We only need to test each 
					// subsequent plane, against subsequent new triangles
					// as all triangles after a plane clip are guaranteed
					// to lie on the inside of the plane. I like how this
					// comment is almost completely and utterly justified
					switch (p)
					{
					case 0:	nTrisToAdd = mv.Triangle_ClipAgainstPlane({ 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, test, clipped[0], clipped[1]); break;
					case 1:	nTrisToAdd = mv.Triangle_ClipAgainstPlane({ 0.0f, (float)ScreenHeight() - 1, 0.0f }, { 0.0f, -1.0f, 0.0f }, test, clipped[0], clipped[1]); break;
					case 2:	nTrisToAdd = mv.Triangle_ClipAgainstPlane({ 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, test, clipped[0], clipped[1]); break;
					case 3:	nTrisToAdd = mv.Triangle_ClipAgainstPlane({ (float)ScreenWidth() - 1, 0.0f, 0.0f }, { -1.0f, 0.0f, 0.0f }, test, clipped[0], clipped[1]); break;
					}

					// Clipping may yield a variable number of triangles, so
					// add these new ones to the back of the queue for subsequent
					// clipping against next planes
					for (int w = 0; w < nTrisToAdd; w++)
						listTriangles.push_back(clipped[w]);
				}
				nNewTriangles = listTriangles.size();
			}


			// Draw the transformed, viewed, clipped, projected, sorted, clipped triangles
			for (auto& t : listTriangles)
			{
				FillTriangle(t.p[0].x, t.p[0].y, t.p[1].x, t.p[1].y, t.p[2].x, t.p[2].y, t.sym, t.col);
				DrawTriangle(t.p[0].x, t.p[0].y, t.p[1].x, t.p[1].y, t.p[2].x, t.p[2].y, PIXEL_SOLID, FG_BLACK);
			}
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