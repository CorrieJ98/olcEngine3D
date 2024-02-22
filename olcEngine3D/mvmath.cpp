#ifndef MV_MATH_CPP
#define MV_MATH_CPP

#include "mvmath.h"
#include <algorithm>

const float HALF_PI = 1.5707963267;
const float PI = 3.1415926535;
const float TAU = 6.2831853071;

void MV_Math::Matrix_MakeIdentity(mat4x4& id)
{
    id.m[0][0] = 1.0f;
    id.m[1][1] = 1.0f;
    id.m[2][2] = 1.0f;
    id.m[3][3] = 1.0f;
}

mat4x4 MV_Math::Matrix_PointAt(v3f& pos, v3f& target, v3f&up)
{// Calculate new forward (x) direction
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

mat4x4 MV_Math::Matrix_QuickInvert(mat4x4& m)
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

mat4x4 MV_Math::Matrix_RotAxisX(float angleRad)
{
	mat4x4 matRotX;
	matRotX.m[0][0] = 1.0f;
	matRotX.m[1][1] = cosf(angleRad);
	matRotX.m[1][2] = sinf(angleRad);
	matRotX.m[2][1] = -sinf(angleRad);
	matRotX.m[2][2] = cosf(angleRad);
	matRotX.m[3][3] = 1.0f;
	return matRotX;
}

mat4x4 MV_Math::Matrix_RotAxisY(float angleRad)
{
	mat4x4 matRotY;
	matRotY.m[0][0] = cosf(angleRad);
	matRotY.m[0][2] = sinf(angleRad);
	matRotY.m[2][0] = -sinf(angleRad);
	matRotY.m[1][1] = 1.0f;
	matRotY.m[2][2] = cosf(angleRad);
	matRotY.m[3][3] = 1.0f;
	return matRotY;
}

mat4x4 MV_Math::Matrix_RotAxisZ(float angleRad)
{
	mat4x4 matRotZ;
	matRotZ.m[0][0] = cosf(angleRad);
	matRotZ.m[0][1] = sinf(angleRad);
	matRotZ.m[1][0] = -sinf(angleRad);
	matRotZ.m[1][1] = cosf(angleRad);
	matRotZ.m[2][2] = 1.0f;
	matRotZ.m[3][3] = 1.0f;
	return matRotZ;
}

mat4x4 MV_Math::Matrix_MakeTranslation(float x, float y, float z)
{
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

mat4x4 MV_Math::Matrix_MakeProjection(float fov, float aspect_ratio, float zNear, float zFar)
{
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

mat4x4 MV_Math::Matrix_MultiplyMatrix(mat4x4& m1, mat4x4& m2)
{
	mat4x4 mout;
	for (int c = 0; c < 4; c++)
		for (int r = 0; r < 4; r++)
			mout.m[r][c] = m1.m[r][0] * m2.m[0][c] + m1.m[r][1] * m2.m[1][c] + m1.m[r][2] * m2.m[2][c] + m1.m[r][3] * m2.m[3][c];
	return mout;
}

v3f MV_Math::Matrix_MultiplyVector(mat4x4& m, v3f& vin)
{
	v3f vout;
	vout.x = vin.x * m.m[0][0] + vin.y * m.m[1][0] + vin.z * m.m[2][0] + vin.w * m.m[3][0];
	vout.y = vin.x * m.m[0][1] + vin.y * m.m[1][1] + vin.z * m.m[2][1] + vin.w * m.m[3][1];
	vout.z = vin.x * m.m[0][2] + vin.y * m.m[1][2] + vin.z * m.m[2][2] + vin.w * m.m[3][2];
	vout.w = vin.x * m.m[0][3] + vin.y * m.m[1][3] + vin.z * m.m[2][3] + vin.w * m.m[3][3];
	return vout;
}

v3f MV_Math::Vector_Add(v3f& v1, v3f& v2)
{
	return { v1.x + v2.x, v1.y + v2.y, v1.z + v2.z };
}

v3f MV_Math::Vector_Subtract(v3f& v1, v3f& v2)
{
	return { v1.x - v2.x, v1.y - v2.y, v1.z - v2.z };
}

v3f MV_Math::Vector_Multiply(v3f& v1, float k) {
	return { v1.x * k, v1.y * k, v1.z * k };
}

v3f MV_Math::Vector_Divide(v3f& v1, float k)
{
	return { v1.x / k, v1.y / k, v1.z / k };
}

float MV_Math::Vector_DotProduct(v3f& v1, v3f& v2)
{
	return { v1.x * v2.x + v1.y * v2.y + v1.z * v2.z };
}

float MV_Math::Vector_Length(v3f& vec)
{
	return sqrtf(Vector_DotProduct(vec, vec));
}

v3f MV_Math::Vector_Normalise(v3f& vec)
{
	float l = Vector_Length(vec);
	return { vec.x / l, vec.y / l, vec.z / l };
}

v3f MV_Math::Vector_CrossProduct(v3f& v1, v3f& v2)
{
	v3f v;
	v.x = v1.y * v2.z - v1.z * v2.y;
	v.y = v1.z * v2.x - v1.x * v2.z;
	v.z = v1.x * v2.y - v1.y * v2.x;
	return v;
}

v3f MV_Math::Vector_IntersectPlane(v3f& plane_p, v3f& plane_n, v3f& lineStart, v3f& lineEnd)
{
	plane_n = Vector_Normalise(plane_n);
	float plane_d = -Vector_DotProduct(plane_n, plane_p);
	float ad = Vector_DotProduct(lineStart, plane_n);
	float bd = Vector_DotProduct(lineEnd, plane_n);
	float t = (-plane_d - ad) / (bd - ad);
	v3f lineStartToEnd = Vector_Subtract(lineEnd, lineStart);
	v3f lineToIntersect = Vector_Multiply(lineStartToEnd, t);
	return Vector_Add(lineStart, lineToIntersect);
}

int MV_Math::Triangle_ClipAgainstPlane(v3f plane_p, v3f plane_n, triangle& in_tri, triangle& out_tri1, triangle& out_tri2)
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

#endif // !MV_MATH_CPP