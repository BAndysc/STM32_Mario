#ifndef VECTOR_H
#define VECTOR_H

#include <stdint.h>

typedef struct {
    float x;
    float y;
} Vectorf;

typedef struct {
    int32_t x;
    int32_t y;
} Vector;

Vector Vectorf2Vector(Vectorf v);
Vector NewVector(int32_t x, int32_t y);
Vectorf NewVectorf(float x, float y);

Vector VectorAdd(Vector v, Vector w);
Vectorf VectorfAdd(Vector v, Vectorf w);
Vectorf VectorffAdd(Vectorf v, Vectorf w);
Vectorf VectorfMul(Vectorf v, float t);

#endif