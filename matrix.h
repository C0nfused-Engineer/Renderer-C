#ifndef MATRIX_H
#define MATRIX_H
#include "vector.h"

typedef struct {
	float m[4][4];
} mat4;

float cot(float x);
mat4 mat4_identity();
mat4 perspective(float FOV, float aspectRatio, float nearCP, float farCP);
mat4 translate(vec3 position);
mat4 scale(vec3 scaleVec);
mat4 rotateX(float angle);
mat4 rotateY(float angle);
mat4 rotateZ(float angle);
mat4 mat4_mul(mat4 a, mat4 b);
vec4 mat4_mul_vec4(mat4 m, vec4 v);
#endif