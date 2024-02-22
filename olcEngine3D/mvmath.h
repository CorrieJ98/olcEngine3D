#ifndef MV_MATH_H
#define MV_MATH_H


#include "vector.cpp"
#include "matrix.cpp"

class MV_Math
{
public:
	void Matrix_MakeIdentity(mat4x4&);
	mat4x4 Matrix_PointAt(v3f&, v3f&, v3f&);
	mat4x4 Matrix_QuickInvert(mat4x4&);
	mat4x4 Matrix_RotAxisX(float);
	mat4x4 Matrix_RotAxisY(float);
	mat4x4 Matrix_RotAxisZ(float);
	mat4x4 Matrix_MakeTranslation(float, float, float);
	mat4x4 Matrix_MakeProjection(float, float, float, float);
	mat4x4 Matrix_MultiplyMatrix(mat4x4&, mat4x4&);
	v3f Matrix_MultiplyVector(mat4x4&, v3f&);
	v3f Vector_Add(v3f&, v3f&);
	v3f Vector_Subtract(v3f&, v3f&);
	v3f Vector_Multiply(v3f&, float);
	v3f Vector_Divide(v3f&, float);
	float Vector_DotProduct(v3f&, v3f&);
	float Vector_Length(v3f&);
	v3f Vector_Normalise(v3f&);
	v3f Vector_CrossProduct(v3f&, v3f&);
	v3f Vector_IntersectPlane(v3f&, v3f&, v3f&, v3f&);
	int Triangle_ClipAgainstPlane(v3f, v3f, triangle&, triangle&, triangle&);



};

#endif // !1