#include <fstream>
#include <strstream>
#include <algorithm>
#include <iostream>

class CJMath {
public:
	const float PI = 3.141592653;

	matrix4x4 Matrix_MakeIdentity() {
		matrix4x4 id;
		id.m[0][0] = 1.0f;
		id.m[1][1] = 1.0f;
		id.m[2][2] = 1.0f;
		id.m[3][3] = 1.0f;
		return id;
	}

	matrix4x4 Matrix_PointAt(vec3d& pos, vec3d& target, vec3d& up) {
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

	matrix4x4 Matrix_QuickInvert(matrix4x4& m) // Only for Rotation/Translation Matrices
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

	matrix4x4 Matrix_MakeProjection(float fov, float aspect_ratio, float zNear, float zFar) {
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

	vec3d Vector_Multiply(vec3d v1, float k) {
		return { v1.x * k, v1.y * k, v1.z * k };
	}

	vec3d Vector_Divide(vec3d& v1, float k) {
		return { v1.x / k, v1.y / k, v1.z / k };
	}

	float Vector_DotProduct(vec3d& v1, vec3d& v2) {
		return { v1.x * v2.x + v1.y * v2.y + v1.z * v2.z };
	}

	float Vector_Length(vec3d& vec) {
		return sqrtf(Vector_DotProduct(vec, vec));
	}

	vec3d Vector_Normalise(vec3d& vec) {
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


	float Q3_InverseSqrt(float x) {
		long i;
		float x2, y;
		const float threehalves = 1.5f;

		x2 = y * 0.5f;
		y = x;
		i = *(long*)&y;
		i = 0x05f3759df - (i >> 1);
		y = *(float*)&i;
		y = y * (threehalves - (x2 * y * y));	// first newtonian iteration
	}
};