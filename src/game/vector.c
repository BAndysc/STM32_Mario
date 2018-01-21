#include "vector.h"

Vector Vectorf2Vector(Vectorf v)
{
	return NewVector((int32_t)v.x, (int32_t)v.y);
}

Vector NewVector(int32_t x, int32_t y)
{
	Vector v = {.x = x, .y = y};
	return v;
}

Vectorf NewVectorf(float x, float y)
{
	Vectorf v = {.x = x, .y = y};
	return v;
}

Vector VectorAdd(Vector v, Vector w)
{
	return NewVector(v.x + w.x, v.y + w.y);
}

Vectorf VectorfAdd(Vector v, Vectorf w)
{
	return NewVectorf(v.x + w.x, v.y + w.y);
}

Vectorf VectorffAdd(Vectorf v, Vectorf w)
{
	return NewVectorf(v.x + w.x, v.y + w.y);
}

Vectorf VectorfMul(Vectorf v, float t)
{
	return NewVectorf(v.x * t, v.y * t);
}