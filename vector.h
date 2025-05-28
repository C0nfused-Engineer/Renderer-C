#ifndef VECTOR_H
#define VECTOR_H

typedef struct vec2 {
	double x, y;
} vec2;

typedef struct vec3 {
	double x, y, z;
} vec3;

typedef struct vec4 {
	double x, y, z, w;
} vec4;

vec2 vec2_add(vec2 a, vec2 b);

vec3 vec3_zero();
vec3 vec3_negation(vec3 a);
vec3 vec3_add(vec3 a, vec3 b);
vec3 vec3_scale(vec3 a, float s);
double vec3_dot(vec3 a, vec3 b);
vec3 vec3_normalize(vec3 v);
vec3 vec3_cross(vec3 a, vec3 b);

vec4 vec4add(vec4 a, vec4 b);

#endif