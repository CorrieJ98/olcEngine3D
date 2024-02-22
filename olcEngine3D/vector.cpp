#ifndef VECTOR_CPP
#define VECTOR_CPP

struct v3f {
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;
	float w = 1.0f;
};

struct triangle {
	v3f p[3];

	wchar_t sym;
	short col;
};

#endif // !VECTOR_CPP