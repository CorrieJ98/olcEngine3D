#include "olcConsoleGameEngine.h"
#include "matrix.cpp"
#include "matrix.h"
#include "vector.cpp"
#include <fstream>
#include <strstream>
#include <algorithm>
#include <iostream>

using namespace std;

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

class Camera {
public:
	Camera();

	v3f pos;
	float pitch;
	float yaw;
	v3f lookAt;

	void Yaw(float angle) {

	}
	void Pitch(float angle) {

	}



	~Camera();
protected:
	v3f fwd;
	v3f up;
	v3f right;
private:
};

class Mesh
{
public:
	Mesh();
	~Mesh();

protected:
	std::string displayName = "unnamed";

private:
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

class GameLevel {
public:
	GameLevel();
	~GameLevel();
protected:
private:

};

class InteractableObject {
public:
	InteractableObject();
	~InteractableObject();
protected:
private:
};

class EquippableObject {
public:
	EquippableObject();
	~EquippableObject();
protected:
private:
};

class Button : InteractableObject {
public:
	Button();
	~Button();

	void PressButton() {

	}


protected:
	bool isLever = false; // false = off -> on forever, true is on/off togglable
private:
};

class Character {
public:
	Character();
	~Character();
protected:
	int maxHP;
	int currentHP;
	float moveSpeed;

private:
	Mesh mesh;
};

class Enemy : Character{
public:
	Enemy();
	~Enemy();
protected:
private:
};

class SpawnPoint {
public:
	SpawnPoint();
	~SpawnPoint();
protected:
private:
};

class olcEngine3D : public olcConsoleGameEngine
{
public:
	olcEngine3D() 
	{
		m_sAppName = L"Tankers 3D";
	}


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

	// ::::: MATRIX & VECTOR MATH :::::
	mat4x4 Matrix_MakeIdentity() {
		mat4x4 id;
		id.m[0][0] = 1.0f;
		id.m[1][1] = 1.0f;
		id.m[2][2] = 1.0f;
		id.m[3][3] = 1.0f;
		return id;
	}
	mat4x4 Matrix_PointAt(v3f &pos, v3f &target, v3f &up){
		// Calculate new forward (x) direction
		v3f newFwd = Vector_Subtract(target, pos);
		newFwd = Vector_Normalise(newFwd);

		// Calculate new up (y) direction
		v3f a = Vector_Multiply(newFwd, Vector_DotProduct(up, newFwd));
		v3f newUp = Vector_Subtract(up, a);
		newUp = Vector_Normalise(newUp);
		
		// Calculate new right(z) direction
		v3f newRight = Vector_CrossProduct(newUp, newFwd);

		// Construct Dimension and Translation Matrix
		mat4x4 m;
		m.m[0][0] = newRight.x;
		m.m[0][1] = newRight.y;
		m.m[0][2] = newRight.z;
		m.m[0][3] = 0.0f;
		m.m[1][0] = newUp.x;
		m.m[1][1] = newUp.y;
		m.m[1][2] = newUp.z;
		m.m[1][3] = 0.0f;
		m.m[2][0] = newFwd.x;
		m.m[2][1] = newFwd.y;
		m.m[2][2] = newFwd.z;
		m.m[2][3] = 0.0f;
		m.m[3][0] = pos.x;
		m.m[3][1] = pos.y;
		m.m[3][2] = pos.z;
		m.m[3][3] = 1.0f;
		return m;
	}
	mat4x4 Matrix_QuickInvert(mat4x4 &m) // Only for Rotation/Translation Matrices
	{
		mat4x4 matrix;
		matrix.m[0][0] = m.m[0][0]; matrix.m[0][1] = m.m[1][0]; matrix.m[0][2] = m.m[2][0]; matrix.m[0][3] = 0.0f;
		matrix.m[1][0] = m.m[0][1]; matrix.m[1][1] = m.m[1][1]; matrix.m[1][2] = m.m[2][1]; matrix.m[1][3] = 0.0f;
		matrix.m[2][0] = m.m[0][2]; matrix.m[2][1] = m.m[1][2]; matrix.m[2][2] = m.m[2][2]; matrix.m[2][3] = 0.0f;
		matrix.m[3][0] = -(m.m[3][0] * matrix.m[0][0] + m.m[3][1] * matrix.m[1][0] + m.m[3][2] * matrix.m[2][0]);
		matrix.m[3][1] = -(m.m[3][0] * matrix.m[0][1] + m.m[3][1] * matrix.m[1][1] + m.m[3][2] * matrix.m[2][1]);
		matrix.m[3][2] = -(m.m[3][0] * matrix.m[0][2] + m.m[3][1] * matrix.m[1][2] + m.m[3][2] * matrix.m[2][2]);
		matrix.m[3][3] = 1.0f;
		return matrix;
	}
	mat4x4 Matrix_RotAxisX(float angleRad) {
		mat4x4 matRotX;
		matRotX.m[0][0] = 1.0f;
		matRotX.m[1][1] = cosf(angleRad);
		matRotX.m[1][2] = sinf(angleRad);
		matRotX.m[2][1] = -sinf(angleRad);
		matRotX.m[2][2] = cosf(angleRad);
		matRotX.m[3][3] = 1.0f;
		return matRotX;
	}
	mat4x4 Matrix_RotAxisZ(float angleRad) {
		mat4x4 matRotZ;
		matRotZ.m[0][0] = cosf(angleRad);
		matRotZ.m[0][1] = sinf(angleRad);
		matRotZ.m[1][0] = -sinf(angleRad);
		matRotZ.m[1][1] = cosf(angleRad);
		matRotZ.m[2][2] = 1.0f;
		matRotZ.m[3][3] = 1.0f;
		return matRotZ;
	}
	mat4x4 Matrix_RotAxisY(float angleRad) {
		mat4x4 matRotY;
		matRotY.m[0][0] = cosf(angleRad);
		matRotY.m[0][2] = sinf(angleRad);
		matRotY.m[2][0] = -sinf(angleRad);
		matRotY.m[1][1] = 1.0f;
		matRotY.m[2][2] = cosf(angleRad);
		matRotY.m[3][3] = 1.0f;
		return matRotY;
	}
	mat4x4 Matrix_MakeTranslate(float x, float y, float z) {
		mat4x4 matT;
		matT.m[0][0] = 1.0f;
		matT.m[1][1] = 1.0f;
		matT.m[2][2] = 1.0f;
		matT.m[3][3] = 1.0f;
		matT.m[3][0] = x;
		matT.m[3][1] = y;
		matT.m[3][2] = z;
		return matT;
	}
	mat4x4 Matrix_MakeProjection(float fov, float aspect_ratio, float zNear, float zFar){
		float fovRad = 1.0f / tanf(fov * 0.5f / 180.0f * PI);
		mat4x4 matProj;
		matProj.m[0][0] = aspect_ratio * fovRad;
		matProj.m[1][1] = fovRad;
		matProj.m[2][2] = zFar / (zFar - zNear);
		matProj.m[3][2] = (-zFar * zNear) / (zFar - zNear);
		matProj.m[2][3] = 1.0f;
		matProj.m[3][3] = 0.0f;
		return matProj;
	}
	mat4x4 Matrix_MultiplyMatrix(mat4x4& m1, mat4x4& m2) {
		mat4x4 mout;
		for (int c = 0; c < 4; c++)
			for (int r = 0; r < 4; r++)
				mout.m[r][c] = m1.m[r][0] * m2.m[0][c] + m1.m[r][1] * m2.m[1][c] + m1.m[r][2] * m2.m[2][c] + m1.m[r][3] * m2.m[3][c];
		return mout;
	}
	v3f Matrix_MultiplyVector(mat4x4& m, v3f& vin) {
		v3f vout;
		vout.x = vin.x * m.m[0][0] + vin.y * m.m[1][0] + vin.z * m.m[2][0] + vin.w * m.m[3][0];
		vout.y = vin.x * m.m[0][1] + vin.y * m.m[1][1] + vin.z * m.m[2][1] + vin.w * m.m[3][1];
		vout.z = vin.x * m.m[0][2] + vin.y * m.m[1][2] + vin.z * m.m[2][2] + vin.w * m.m[3][2];
		vout.w = vin.x * m.m[0][3] + vin.y * m.m[1][3] + vin.z * m.m[2][3] + vin.w * m.m[3][3];
		return vout;
	}
	v3f Vector_Add(v3f& v1, v3f& v2) {
		return { v1.x + v2.x, v1.y + v2.y, v1.z + v2.z };
	}
	v3f Vector_Subtract(v3f& v1, v3f& v2) {
		return { v1.x - v2.x, v1.y - v2.y, v1.z - v2.z };
	}
	v3f Vector_Multiply(v3f& v1, float k) {
		return { v1.x * k, v1.y * k, v1.z * k };
	}
	v3f Vector_Divide(v3f& v1,float k) {
		return { v1.x / k, v1.y / k, v1.z / k };
	}
	float Vector_DotProduct(v3f& v1, v3f& v2) {
		return { v1.x * v2.x + v1.y * v2.y + v1.z * v2.z };
	}
	float Vector_Length(v3f& vec) {
		return sqrtf(Vector_DotProduct(vec, vec));
	}
	v3f Vector_Normalise(v3f &vec) {
		float l = Vector_Length(vec);
		return { vec.x / l, vec.y / l, vec.z / l };
	}
	v3f Vector_CrossProduct(v3f& v1, v3f& v2) {
		v3f v;
		v.x = v1.y * v2.z - v1.z * v2.y;
		v.y = v1.z * v2.x - v1.x * v2.z;
		v.z = v1.x * v2.y - v1.y * v2.x;
		return v;
	}

	v3f Vector_IntersectPlane(v3f& plane_p, v3f& plane_n, v3f& lineStart, v3f& lineEnd) {
		plane_n = Vector_Normalise(plane_n);
		float plane_d = -Vector_DotProduct(plane_n, plane_p);
		float ad = Vector_DotProduct(lineStart, plane_n);
		float bd = Vector_DotProduct(lineEnd, plane_n);
		float t = (-plane_d - ad) / (bd - ad);
		v3f lineStartToEnd = Vector_Subtract(lineEnd, lineStart);
		v3f lineToIntersect = Vector_Multiply(lineStartToEnd, t);
		return Vector_Add(lineStart, lineToIntersect);
	}
	int Triangle_ClipAgainstPlane(v3f plane_p, v3f plane_n, triangle& in_tri, triangle& out_tri1, triangle& out_tri2)
	{
		// Make sure plane normal is indeed normal
		plane_n = Vector_Normalise(plane_n);

		// Return signed shortest distance from point to plane, plane normal must be normalised
		auto dist = [&](v3f& p)
			{
				v3f n = Vector_Normalise(p);
				return (plane_n.x * p.x + plane_n.y * p.y + plane_n.z * p.z - Vector_DotProduct(plane_n, plane_p));
			};

		// Create two temporary storage arrays to classify points either side of plane
		// If distance sign is positive, point lies on "inside" of plane
		v3f* inside_points[3];  int nInsidePointCount = 0;
		v3f* outside_points[3]; int nOutsidePointCount = 0;

		// Get signed distance of each point in triangle to plane
		float d0 = dist(in_tri.p[0]);
		float d1 = dist(in_tri.p[1]);
		float d2 = dist(in_tri.p[2]);

		if (d0 >= 0) { inside_points[nInsidePointCount++] = &in_tri.p[0]; }
		else { outside_points[nOutsidePointCount++] = &in_tri.p[0]; }
		if (d1 >= 0) { inside_points[nInsidePointCount++] = &in_tri.p[1]; }
		else { outside_points[nOutsidePointCount++] = &in_tri.p[1]; }
		if (d2 >= 0) { inside_points[nInsidePointCount++] = &in_tri.p[2]; }
		else { outside_points[nOutsidePointCount++] = &in_tri.p[2]; }

		// Now classify triangle points, and break the input triangle into 
		// smaller output triangles if required. There are four possible
		// outcomes...

		if (nInsidePointCount == 0)
		{
			// All points lie on the outside of plane, so clip whole triangle
			// It ceases to exist

			return 0; // No returned triangles are valid
		}

		if (nInsidePointCount == 3)
		{
			// All points lie on the inside of plane, so do nothing
			// and allow the triangle to simply pass through
			out_tri1 = in_tri;

			return 1; // Just the one returned original triangle is valid
		}

		if (nInsidePointCount == 1 && nOutsidePointCount == 2)
		{
			// Triangle should be clipped. As two points lie outside
			// the plane, the triangle simply becomes a smaller triangle

			// Copy appearance info to new triangle
			out_tri1.col = in_tri.col;
			out_tri1.sym = in_tri.sym;

			// The inside point is valid, so keep that...
			out_tri1.p[0] = *inside_points[0];

			// but the two new points are at the locations where the 
			// original sides of the triangle (lines) intersect with the plane
			out_tri1.p[1] = Vector_IntersectPlane(plane_p, plane_n, *inside_points[0], *outside_points[0]);
			out_tri1.p[2] = Vector_IntersectPlane(plane_p, plane_n, *inside_points[0], *outside_points[1]);

			return 1; // Return the newly formed single triangle
		}

		if (nInsidePointCount == 2 && nOutsidePointCount == 1)
		{
			// Triangle should be clipped. As two points lie inside the plane,
			// the clipped triangle becomes a "quad". Fortunately, we can
			// represent a quad with two new triangles

			// Copy appearance info to new triangles
			out_tri1.col = in_tri.col;
			out_tri1.sym = in_tri.sym;

			out_tri2.col = in_tri.col;
			out_tri2.sym = in_tri.sym;

			// The first triangle consists of the two inside points and a new
			// point determined by the location where one side of the triangle
			// intersects with the plane
			out_tri1.p[0] = *inside_points[0];
			out_tri1.p[1] = *inside_points[1];
			out_tri1.p[2] = Vector_IntersectPlane(plane_p, plane_n, *inside_points[0], *outside_points[0]);

			// The second triangle is composed of one of he inside points, a
			// new point determined by the intersection of the other side of the 
			// triangle and the plane, and the newly created point above
			out_tri2.p[0] = *inside_points[1];
			out_tri2.p[1] = out_tri1.p[2];
			out_tri2.p[2] = Vector_IntersectPlane(plane_p, plane_n, *inside_points[1], *outside_points[0]);

			return 2; // Return two newly formed triangles which form a quad
		}
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
		projection_matrix = Matrix_MakeProjection(90.0f, (float)ScreenHeight() / (float)ScreenWidth(), 0.01f, 1000.0f);
		return true;
	}

	bool OnUserUpdate(float elapsedTime) override
	{
		v3f up = { 0.0f,1.0f,0.0f };
		v3f target = { 0.0f,0.0f,1.0f };

		mat4x4 ninetyY = Matrix_RotAxisY(0.5f * PI);
		mat4x4 ninetyX = Matrix_RotAxisX(1.5f * PI);
		v3f r = Matrix_MultiplyVector(ninetyY, lookdir);
		v3f u = Matrix_MultiplyVector(ninetyX, lookdir);
		v3f vForward = Vector_Multiply(lookdir, 8.0f * elapsedTime);
		v3f vShoulderRail = Vector_Multiply(r, 8.0f * elapsedTime);
		v3f vBackRail = Vector_Multiply(u, 8.0f * elapsedTime);

		mat4x4 y = Matrix_RotAxisY(eye.yaw);
		mat4x4 p = Matrix_RotAxisX(eye.pitch);

		mat4x4 camMatrixRot = Matrix_RotAxisY(eye.yaw);
		lookdir = Matrix_MultiplyVector(camMatrixRot, target);
		target = Vector_Add(eye.pos, lookdir);
		mat4x4 camMatrix = Matrix_PointAt(eye.pos, target, up);
		mat4x4 matView = Matrix_QuickInvert(camMatrix);

		// ::::: ROTATION MATRIX :::::
		mat4x4 matRotY, matRotX;
		matRotY = Matrix_RotAxisY(eye.yaw);
		matRotX = Matrix_RotAxisX(eye.pitch);

		mat4x4 matTranslation;
		matTranslation = Matrix_MakeTranslate(0.0f, 0.0f, camZOffset);

		mat4x4 matWorld;
		matWorld = Matrix_MakeIdentity();
		matWorld = Matrix_MultiplyMatrix(matRotY, matRotX);
		matWorld = Matrix_MultiplyMatrix(matWorld, matTranslation);



		// ::::: KEYBINDINGS :::::

		// Movement
		if (GetKey(L'E').bHeld)	// Up
			eye.pos.y += 8.0f * elapsedTime;

		if (GetKey(L'Q').bHeld)	// Down
			eye.pos.y -= 8.0f * elapsedTime;

		if (GetKey(L'A').bHeld)	// Left
			eye.pos = Vector_Subtract(eye.pos, vShoulderRail);

		if (GetKey(L'D').bHeld)	// Right
			eye.pos = Vector_Add(eye.pos, vShoulderRail);

		if (GetKey(L'W').bHeld)	// Forward
			eye.pos = Vector_Add(eye.pos, vForward);

		if (GetKey(L'S').bHeld) // Back
			eye.pos = Vector_Subtract(eye.pos, vForward);


		// Camera Keyboard
		if (GetKey(VK_LEFT).bHeld)	// Yaw Left
			eye.yaw -= 2.0f * elapsedTime;

		if (GetKey(VK_RIGHT).bHeld)	// Yaw Right
			eye.yaw += 2.0f * elapsedTime;

		if (GetKey(VK_UP).bHeld)	// Pitch Up
			eye.pitch -= 2.0f * elapsedTime;

		if (GetKey(VK_DOWN).bHeld)	// Pitch Down
			eye.pitch += 2.0f * elapsedTime;

		// Camera Mouse
		CursorManager(GetMouseX(), GetMouseY(), mouseX, mouseY, 1.0f);
		if (mouseX != (ScreenWidth() >> 1)) {
			eye.yaw += mouseX;
		}

		if (mouseY != (ScreenHeight() >> 1)) {
			eye.pitch += mouseY;
		};


		// Clear screen
		Fill(0, 0, ScreenWidth(), ScreenHeight(), PIXEL_SOLID, FG_BLACK);


		vector<triangle> vecTrianglesToRaster;


		// Draw Triangles
		for (auto tri : testMesh.tris)
		{
			triangle triProjected, triTransformed, triViewed;

			// World Matrix Transform
			triTransformed.p[0] = Matrix_MultiplyVector(matWorld, tri.p[0]);
			triTransformed.p[1] = Matrix_MultiplyVector(matWorld, tri.p[1]);
			triTransformed.p[2] = Matrix_MultiplyVector(matWorld, tri.p[2]);

			// Calculate triangle Normal
			v3f normal, line1, line2;

			// Get lines either side of triangle
			line1 = Vector_Subtract(triTransformed.p[1], triTransformed.p[0]);
			line2 = Vector_Subtract(triTransformed.p[2], triTransformed.p[0]);

			// Take cross product of lines to get normal to triangle surface
			normal = Vector_CrossProduct(line1, line2);

			// You normally need to normalise a normal!
			normal = Vector_Normalise(normal);

			// Get Ray from triangle to camera
			v3f vCameraRay = Vector_Subtract(triTransformed.p[0], eye.pos);

			// If ray is aligned with normal, then triangle is visible
			if (Vector_DotProduct(normal, vCameraRay) < 0.0f)
			{
				// Illumination
				v3f light_direction = { 1.0f, 1.5f, 2.0f };
				light_direction = Vector_Normalise(light_direction);

				// How "aligned" are light direction and triangle surface normal?
				float dp = max(0.1f, Vector_DotProduct(light_direction, normal));

				// Choose console colours as required (much easier with RGB)
				CHAR_INFO c = GetColour(dp);
				triTransformed.col = c.Attributes;
				triTransformed.sym = c.Char.UnicodeChar;

				// Convert World Space --> View Space
				triViewed.p[0] = Matrix_MultiplyVector(matView, triTransformed.p[0]);
				triViewed.p[1] = Matrix_MultiplyVector(matView, triTransformed.p[1]);
				triViewed.p[2] = Matrix_MultiplyVector(matView, triTransformed.p[2]);
				triViewed.sym = triTransformed.sym;
				triViewed.col = triTransformed.col;

				// Clip Viewed Triangle against near plane, this could form two additional
				// additional triangles. 
				int nClippedTriangles = 0;
				triangle clipped[2];
				nClippedTriangles = Triangle_ClipAgainstPlane({ 0.0f, 0.0f, 0.1f }, { 0.0f, 0.0f, 1.0f }, triViewed, clipped[0], clipped[1]);

				// We may end up with multiple triangles form the clip, so project as
				// required
				for (int n = 0; n < nClippedTriangles; n++)
				{
					// Project triangles from 3D --> 2D
					triProjected.p[0] = Matrix_MultiplyVector(projection_matrix, clipped[n].p[0]);
					triProjected.p[1] = Matrix_MultiplyVector(projection_matrix, clipped[n].p[1]);
					triProjected.p[2] = Matrix_MultiplyVector(projection_matrix, clipped[n].p[2]);
					triProjected.col = clipped[n].col;
					triProjected.sym = clipped[n].sym;

					// Scale into view, we moved the normalising into cartesian space
					// out of the matrix.vector function from the previous videos, so
					// do this manually
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

					// Offset verts into visible normalised space
					v3f vOffsetView = { 1,1,0 };
					triProjected.p[0] = Vector_Add(triProjected.p[0], vOffsetView);
					triProjected.p[1] = Vector_Add(triProjected.p[1], vOffsetView);
					triProjected.p[2] = Vector_Add(triProjected.p[2], vOffsetView);
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
					case 0:	nTrisToAdd = Triangle_ClipAgainstPlane({ 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, test, clipped[0], clipped[1]); break;
					case 1:	nTrisToAdd = Triangle_ClipAgainstPlane({ 0.0f, (float)ScreenHeight() - 1, 0.0f }, { 0.0f, -1.0f, 0.0f }, test, clipped[0], clipped[1]); break;
					case 2:	nTrisToAdd = Triangle_ClipAgainstPlane({ 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, test, clipped[0], clipped[1]); break;
					case 3:	nTrisToAdd = Triangle_ClipAgainstPlane({ (float)ScreenWidth() - 1, 0.0f, 0.0f }, { -1.0f, 0.0f, 0.0f }, test, clipped[0], clipped[1]); break;
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
	olcEngine3D tankers;
	if (tankers.ConstructConsole(256, 240, 4, 4))
		tankers.Start();
    return 0;
}