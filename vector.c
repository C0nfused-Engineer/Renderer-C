#include <math.h>
#include "vector.h"

//Vector 2 Operations
vec2 vec2_add(vec2 a, vec2 b) {
	return (vec2){a.x + b.x, a.y + b.y};
}

//Vector 3 Operations
vec3 vec3_zero() {
	return (vec3){0.0f, 0.0f, 0.0f};
}

vec3 vec3_negation(vec3 a) {
	return (vec3){-a.x, -a.y, -a.z};
}

vec3 vec3_add(vec3 a, vec3 b) {
	return (vec3){a.x + b.x, a.y + b.y, a.z + b.z};
}

vec3 vec3_scale(vec3 a, float s) {
	return (vec3){a.x * s, a.y * s, a.z * s};
}

double vec3_dot(vec3 a, vec3 b) {
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

vec3 vec3_cross(vec3 a, vec3 b) {
	return (vec3) {
		a.y * b.z - a.z * b.y,
		a.z * b.x - a.x * b.z,
		a.x * b.y - a.y * b.z
	};
}

vec3 vec3_normalize(vec3 v) {
	double length = sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
	if (length == 0) {
		return (vec3){0.0, 0.0, 0.0};
	}
	
	return (vec3){v.x / length, v.y / length, v.z / length};
}

//Vector 4 Operations
vec4 vec4add(vec4 a, vec4 b) {
	return (vec4){a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w};
}