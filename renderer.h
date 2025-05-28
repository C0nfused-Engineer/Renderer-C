#ifndef RENDERER_H
#define RENDERER_H

#include <SDL3/SDL.h>
#include "object.h"
#include "vector.h"
#include "matrix.h"
#include "queue.h"

typedef struct Object Object;

typedef struct Triangle {
	int a, b, c;
} Triangle;

typedef struct Mesh3D{
	vec3* vertices;
	int vertexCount;
	Triangle* triangles;
	int triangleCount;
} Mesh3D;

typedef struct RenderData{
	Mesh3D* mesh;
} RenderData;

typedef struct SortedTriangle {
    int x[3], y[3]; // Screen coordinates
    float depth;    // Average Z
} SortedTriangle;

//Render Pipeline Functions
void RenderPass(SDL_Renderer *renderer, SDL_Texture* frameBuffer, Queue *renderQueue, Object* camera, float fovDeg, float width, float height);
mat4 BuildView(Object* camera);
mat4 BuildModel(Object* model);
int CullTriangle(vec3 p0, vec3 p1, vec3 p2, Object* camera);
int ProjectTriangle(Triangle tri, Mesh3D* mesh, mat4 mvp, int width, int height, SortedTriangle* out);
void SortTriangles(SortedTriangle* tris, int count);
void DrawTriangles(SDL_Renderer* renderer, SortedTriangle* tris, int count);
mat4 BuildProjection(float fovDeg, float width, float height);
void RasterizeTriangleToBuffer(vec2 p0, vec2 p1, vec2 p2, uint32_t color, uint32_t* pixels, int pitch, int width, int height);

//Camera Handling Functions
void handleCameraMouseLook(SDL_Event *e, Object* camera, _Bool *mouseLocked, SDL_Window* window);
void handleCameraMovement(Object* camera);

//Data Handling Functions
int LoadMeshFromFile(const char *filename, Mesh3D *mesh);
RenderData* defaultRendData();

#endif