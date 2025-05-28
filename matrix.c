#include "matrix.h"
#include <math.h>

float cot(float x) {
	return 1.0f / tanf(x);
}

mat4 mat4_identity() {
	mat4 result = {0};
	for (int i = 0; i < 4; i++) {
		result.m[i][i] = 1.0f;
	}
	return result;
}

mat4 perspective(float FOV, float aspectRatio, float nearCP, float farCP) {
	float FOVrad = FOV * (M_PI / 180);
	
	mat4 pMatrix = {{
		{(1/aspectRatio) * cot(FOVrad / 2), 0, 0, 0},
		{0, cot(FOVrad / 2), 0, 0},
		{0, 0, -(farCP + nearCP) / (farCP - nearCP), -(2 * farCP * nearCP) / (farCP - nearCP)},
		{0, 0, -1, 0}
	}};
	
	return pMatrix;
}

mat4 translate(vec3 position) {
	mat4 result = mat4_identity();
	result.m[0][3] = position.x;
	result.m[1][3] = position.y;
	result.m[2][3] = position.z;
	
	return result;
}

mat4 rotateX(float angle) {
    mat4 result = mat4_identity();
    float c = cosf(angle);
    float s = sinf(angle);

    result.m[1][1] = c;
    result.m[1][2] = -s;
    result.m[2][1] = s;
    result.m[2][2] = c;

    return result;
}

mat4 rotateY(float angle) {
    mat4 result = mat4_identity();
    float c = cosf(angle);
    float s = sinf(angle);

    result.m[0][0] = c;
    result.m[0][2] = s;
    result.m[2][0] = -s;
    result.m[2][2] = c;

    return result;
}

mat4 rotateZ(float angle) {
    mat4 result = mat4_identity();
    float c = cosf(angle);
    float s = sinf(angle);

    result.m[0][0] = c;
    result.m[0][1] = -s;
    result.m[1][0] = s;
    result.m[1][1] = c;

    return result;
}

mat4 scale(vec3 scaleVec) {
    mat4 result = mat4_identity();
    result.m[0][0] = scaleVec.x;
    result.m[1][1] = scaleVec.y;
    result.m[2][2] = scaleVec.z;
    return result;
}

mat4 mat4_mul(mat4 a, mat4 b) {
    mat4 result = {0};
    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            for (int k = 0; k < 4; k++) {
                result.m[row][col] += a.m[row][k] * b.m[k][col];
            }
        }
    }
    return result;
}

vec4 mat4_mul_vec4(mat4 m, vec4 v) {
    vec4 result = {0, 0, 0, 0};
    result.x = m.m[0][0] * v.x + m.m[0][1] * v.y + m.m[0][2] * v.z + m.m[0][3] * v.w;
    result.y = m.m[1][0] * v.x + m.m[1][1] * v.y + m.m[1][2] * v.z + m.m[1][3] * v.w;
    result.z = m.m[2][0] * v.x + m.m[2][1] * v.y + m.m[2][2] * v.z + m.m[2][3] * v.w;
    result.w = m.m[3][0] * v.x + m.m[3][1] * v.y + m.m[3][2] * v.z + m.m[3][3] * v.w;
    return result;
}