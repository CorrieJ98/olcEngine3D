#ifndef VECTOR_CPP
#define VECTOR_CPP


struct v3f {
	float x, y, z;
	float w = 1.0f;
};

struct triangle {
	v3f p[3];

	wchar_t sym;
	short col;
};

#endif // !VECTOR_CPP