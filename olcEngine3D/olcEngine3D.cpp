#include "olcConsoleGameEngine.h"
#include <fstream>
#include <strstream>
#include <algorithm>
#include <iostream>

using namespace std;

struct vec3 {
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;
	float w = 1.0f;
};
struct triangle {
	vec3 p[3];

	wchar_t sym;
	short col;
};
struct matrix4x4 {
	float m[4][4] = { 0 };
};
struct camera {
	vec3 pos;
	float pitch, yaw;

	// Im certain this is a terrible idea
	float pcos = cosf(pitch);
	float psin = sinf(pitch);
	float ycos = cosf(yaw);
	float ysin = sinf(yaw);
};
struct mesh {

	vector<triangle> tris;

	bool LoadFromObjectFile(string sFilename)
	{
		ifstream file(sFilename);
		if (!file.is_open())
			return false;

		// Local cache of verts
		vector<vec3> verts;

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
				vec3 v;
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

static class Camera {
public:
	Camera();

	vec3 pos;
	float pitch = 0.0f;
	float yaw = 0.0f;
	float offset = 6.0f;

	void Yaw(float angle) {

	}
	void Pitch(float angle) {

	}

	~Camera();
protected:
	// Left hand rule
	vec3 fwd;
	vec3 up;
	vec3 right;
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
		vector<vec3> verts;

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
				vec3 v;
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

class Door : InteractableObject {
public:
	Door();
	~Door();

	void UnlockDoor() {
		// take keyID and check for unlock bool
	}

	void OpenDoor(bool* isUnlocked) {
		// open door
	}

protected:
	bool isUnlocked = true;
	bool isVertSliding = false; // true = slides upwards on open, false = slides right on open
	bool isRemote = false; // true = opens with button, false = opens with interact
private:
};

class Key : Door {
public:
	Key();
	~Key();

	void LinkDoorToKey(Door* door, Key* key) {

	}

protected:
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
	matrix4x4 projection_matrix;
	matrix4x4 identity_matrix;
	camera eye;
	vec3 lookdir;
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
	matrix4x4 Matrix_MakeIdentity() {
		matrix4x4 id;
		id.m[0][0] = 1.0f;
		id.m[1][1] = 1.0f;
		id.m[2][2] = 1.0f;
		id.m[3][3] = 1.0f;
		return id;
	}
	matrix4x4 Matrix_PointAt(vec3 &pos, vec3 &target, vec3 &up){
		// Calculate new forward (x) direction
		vec3 newFwd = Vector_Subtract(target, pos);
		newFwd = Vector_Normalise(newFwd);

		// Calculate new up (y) direction
		vec3 a = Vector_Multiply(newFwd, Vector_DotProduct(up, newFwd));
		vec3 newUp = Vector_Subtract(up, a);
		newUp = Vector_Normalise(newUp);
		
		// Calculate new right(z) direction
		vec3 newRight = Vector_CrossProduct(newUp, newFwd);

		// Construct Dimension and Translation Matrix
		matrix4x4 m;
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
	vec3 Matrix_MultiplyVector(matrix4x4& m, vec3& vin) {
		vec3 vout;
		vout.x = vin.x * m.m[0][0] + vin.y * m.m[1][0] + vin.z * m.m[2][0] + vin.w * m.m[3][0];
		vout.y = vin.x * m.m[0][1] + vin.y * m.m[1][1] + vin.z * m.m[2][1] + vin.w * m.m[3][1];
		vout.z = vin.x * m.m[0][2] + vin.y * m.m[1][2] + vin.z * m.m[2][2] + vin.w * m.m[3][2];
		vout.w = vin.x * m.m[0][3] + vin.y * m.m[1][3] + vin.z * m.m[2][3] + vin.w * m.m[3][3];
		return vout;
	}
	vec3 Vector_Add(vec3& v1, vec3& v2) {
		return { v1.x + v2.x, v1.y + v2.y, v1.z + v2.z };
	}
	vec3 Vector_Subtract(vec3& v1, vec3& v2) {
		return { v1.x - v2.x, v1.y - v2.y, v1.z - v2.z };
	}
	vec3 Vector_Multiply(vec3& v1, float k) {
		return { v1.x * k, v1.y * k, v1.z * k };
	}
	vec3 Vector_Divide(vec3& v1,float k) {
		return { v1.x / k, v1.y / k, v1.z / k };
	}
	float Vector_DotProduct(vec3& v1, vec3& v2) {
		return { v1.x * v2.x + v1.y * v2.y + v1.z * v2.z };
	}
	float Vector_Length(vec3& vec) {
		return sqrtf(Vector_DotProduct(vec, vec));
	}
	vec3 Vector_Normalise(vec3 &vec) {
		float l = Vector_Length(vec);
		return { vec.x / l, vec.y / l, vec.z / l };
	}
	vec3 Vector_CrossProduct(vec3& v1, vec3& v2) {
		vec3 v;
		v.x = v1.y * v2.z - v1.z * v2.y;
		v.y = v1.z * v2.x - v1.x * v2.z;
		v.z = v1.x * v2.y - v1.y * v2.x;
		return v;
	}
	float Q_rsqrt(float x) {
		// result of 1/sqrt(x) (inverse square root)
		long i;
		float x2, y;
		const float threehalves = 1.5f;

		x2 = y * 0.5f;
		y = x;
		i = *(long*)&y;
		i = 0x05f3759df - (i >> 1);
		y = *(float*)&i;
		y = y * (threehalves - (x2 * y * y));	// first newtonian iteration
	//	y = y * (threehalves - (x2 * y * y));	// second newtonian iteration (obsolete)
	}


	vec3 Vector_IntersectPlane(vec3& plane_p, vec3& plane_n, vec3& lineStart, vec3& lineEnd) {
		plane_n = Vector_Normalise(plane_n);
		float plane_d = -Vector_DotProduct(plane_n, plane_p);
		float ad = Vector_DotProduct(lineStart, plane_n);
		float bd = Vector_DotProduct(lineEnd, plane_n);
		float t = (-plane_d - ad) / (bd - ad);
		vec3 lineStartToEnd = Vector_Subtract(lineEnd, lineStart);
		vec3 lineToIntersect = Vector_Multiply(lineStartToEnd, t);
		return Vector_Add(lineStart, lineToIntersect);
	}
	int Triangle_ClipAgainstPlane(vec3 plane_p, vec3 plane_n, triangle& in_tri, triangle& out_tri1, triangle& out_tri2)
	{
		// Make sure plane normal is indeed normal
		plane_n = Vector_Normalise(plane_n);

		// Return signed shortest distance from point to plane, plane normal must be normalised
		auto dist = [&](vec3& p)
			{
				vec3 n = Vector_Normalise(p);
				return (plane_n.x * p.x + plane_n.y * p.y + plane_n.z * p.z - Vector_DotProduct(plane_n, plane_p));
			};

		// Create two temporary storage arrays to classify points either side of plane
		// If distance sign is positive, point lies on "inside" of plane
		vec3* inside_points[3];  int nInsidePointCount = 0;
		vec3* outside_points[3]; int nOutsidePointCount = 0;

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
		vec3 up = { 0.0f,1.0f,0.0f };
		vec3 target = { 0.0f,0.0f,1.0f };

		matrix4x4 ninetyY = Matrix_RotAxisY(0.5f * PI);
		matrix4x4 ninetyX = Matrix_RotAxisX(1.5f * PI);
		vec3 r = Matrix_MultiplyVector(ninetyY, lookdir);
		vec3 u = Matrix_MultiplyVector(ninetyX, lookdir);
		vec3 vForward = Vector_Multiply(lookdir, 8.0f * elapsedTime);
		vec3 vShoulderRail = Vector_Multiply(r, 8.0f * elapsedTime);
		vec3 vBackRail = Vector_Multiply(u, 8.0f * elapsedTime);

		matrix4x4 y = Matrix_RotAxisY(eye.yaw);
		matrix4x4 p = Matrix_RotAxisX(eye.pitch);

		matrix4x4 camMatrixRot = Matrix_RotAxisY(eye.yaw);
		lookdir = Matrix_MultiplyVector(camMatrixRot, target);
		target = Vector_Add(eye.pos, lookdir);
		matrix4x4 camMatrix = Matrix_PointAt(eye.pos, target, up);
		matrix4x4 matView = Matrix_QuickInvert(camMatrix);

		// ::::: ROTATION MATRIX :::::
		matrix4x4 matRotY, matRotX;
		matRotY = Matrix_RotAxisZ(eye.yaw);
		matRotX = Matrix_RotAxisX(eye.pitch);

		matrix4x4 matTranslation;
		matTranslation = Matrix_MakeTranslate(0.0f, 0.0f, camZOffset);

		matrix4x4 matWorld,
		identity_matrix = Matrix_MakeIdentity();
		matWorld = Matrix_MakeIdentity();
		matWorld = Matrix_MultiplyMatrix(matWorld, matRotX);
		matWorld = Matrix_MultiplyMatrix(identity_matrix,matRotY);
		matWorld = Matrix_MultiplyMatrix(matWorld, matTranslation);



		// ::::: KEYBINDINGS :::::
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

		if (GetKey(VK_LEFT).bHeld)	// Yaw Left
			eye.yaw -= 2.0f * elapsedTime;

		if (GetKey(VK_RIGHT).bHeld)	// Yaw Right
			eye.yaw += 2.0f * elapsedTime;

		if (GetKey(VK_UP).bHeld)	// Pitch Up
			eye.pitch -= 2.0f * elapsedTime;

		if (GetKey(VK_DOWN).bHeld)	// Pitch Down
			eye.pitch += 2.0f * elapsedTime;


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
			vec3 normal, line1, line2;

			// Get lines either side of triangle
			line1 = Vector_Subtract(triTransformed.p[1], triTransformed.p[0]);
			line2 = Vector_Subtract(triTransformed.p[2], triTransformed.p[0]);

			// Take cross product of lines to get normal to triangle surface
			normal = Vector_CrossProduct(line1, line2);

			// You normally need to normalise a normal!
			normal = Vector_Normalise(normal);

			// Get Ray from triangle to camera
			vec3 vCameraRay = Vector_Subtract(triTransformed.p[0], eye.pos);

			// If ray is aligned with normal, then triangle is visible
			if (Vector_DotProduct(normal, vCameraRay) < 0.0f)
			{
				// Illumination
				vec3 light_direction = { 1.0f, 1.5f, 2.0f };
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
					vec3 vOffsetView = { 1,1,0 };
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