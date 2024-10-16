#include "mvmath.h"

mat4x4 MV_Math::Matrix_MakeIdentity()
{
	mat4x4 id;
	id.m[0][0] = 1.0f;
	id.m[1][1] = 1.0f;
	id.m[2][2] = 1.0f;
	id.m[3][3] = 1.0f;
	return id;
}

mat4x4 MV_Math::Matrix_PointAt(v3f& m_pos, v3f& m_target, v3f& m_up)
{
	// Calculate new forward(x) direction
	v3f newFwd = Vector_Subtract(m_target, m_pos);
	newFwd = Vector_Normalise(newFwd);

	// Calculate new up (y) direction
	v3f a = Vector_Multiply(newFwd, Vector_DotProduct(m_up, newFwd));
	v3f newUp = Vector_Subtract(m_up, a);
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
	m.m[3][0] = m_pos.x;
	m.m[3][1] = m_pos.y;
	m.m[3][2] = m_pos.z;
	m.m[3][3] = 1.0f;
	return m;
}

mat4x4 MV_Math::Matrix_QuickInvert(mat4x4&)
{
	return mat4x4();
}

mat4x4 MV_Math::Matrix_RotAxisX(float m_angle)
{
	return mat4x4();
}

mat4x4 MV_Math::Matrix_RotAxisY(float m_angle)
{
	return mat4x4();
}

mat4x4 MV_Math::Matrix_RotAxisZ(float m_angle)
{
	return mat4x4();
}

mat4x4 MV_Math::Matrix_MakeTranslation(float x, float y, float z)
{
	return mat4x4();
}

mat4x4 MV_Math::Matrix_MakeProjection(float, float, float, float)
{
	return mat4x4();
}

mat4x4 MV_Math::Matrix_MultiplyMatrix(mat4x4& m_lhs, mat4x4& m_rhs)
{
	return mat4x4();
}

v3f MV_Math::Matrix_MultiplyVector(mat4x4& m_m, v3f& m_v)
{
	return v3f();
}

v3f MV_Math::Vector_Add(v3f& m_lhs, v3f& m_rhs)
{
	return v3f();
}

v3f MV_Math::Vector_Subtract(v3f& m_lhs, v3f& m_rhs)
{
	return v3f();
}

v3f MV_Math::Vector_Multiply(v3f& m_v, float m_k)
{
	return v3f();
}

v3f MV_Math::Vector_Divide(v3f& m_v, float m_k)
{
	return v3f();
}

float MV_Math::Vector_DotProduct(v3f& m_lhs, v3f& m_rhs)
{
	return 0.0f;
}

float MV_Math::Vector_Length(v3f& m_v)
{
	return 0.0f;
}

v3f MV_Math::Vector_Normalise(v3f& m_v)
{
	return v3f();
}

v3f MV_Math::Vector_CrossProduct(v3f& m_lhs, v3f& m_rhs)
{
	return v3f();
}

v3f MV_Math::Vector_IntersectPlane(v3f& m_plane_p, v3f& m_plane_n, v3f& m_lineStart, v3f& m_lineEnd)
{
	return v3f();
}

int MV_Math::Triangle_ClipAgainstPlane(v3f m_plane_p, v3f m_plane_n, triangle& m_in_tri, triangle& m_out_tri1, triangle& m_out_tri2)
{
	return 0;
}
