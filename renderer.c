#include "renderer.h"
#include <SDL3/SDL.h>
#include "vector.h"
#include "queue.h"
#include "object.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define WIREFRAME_MODE 0
#define NORMAL_VISUALIZER 1

void RenderPass(SDL_Renderer* renderer, SDL_Texture* frameBuffer, Queue* renderQueue, Object* camera, float fovDeg, float width, float height) {
    mat4 proj = BuildProjection(fovDeg, width, height);
    mat4 view = BuildView(camera);

	// ----- Lock framebuffer -----
    uint32_t* pixels;
    int pitch;
    SDL_LockTexture(frameBuffer, NULL, (void**)&pixels, &pitch);

    // Clear framebuffer to black
    int totalPixels = (pitch / 4) * height;
    for (int i = 0; i < totalPixels; i++) {
        pixels[i] = 0xFF000000; // ARGB -> opaque black
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SortedTriangle* sortedTris = NULL;
    int triCapacity = 0;
    int triCount = 0;

    while (hasNext(renderQueue)) {
        Object* object = (Object*)dequeue(renderQueue);
        if (!object->isRendered || !object->rendData || !object->rendData->mesh) continue;

        Mesh3D* mesh = object->rendData->mesh;
        mat4 model = BuildModel(object);
        mat4 mvp = mat4_mul(proj, mat4_mul(view, model));

        for (int i = 0; i < mesh->triangleCount; ++i) {
            Triangle tri = mesh->triangles[i];

            vec4 v0 = mat4_mul_vec4(model, (vec4){mesh->vertices[tri.a].x, mesh->vertices[tri.a].y, mesh->vertices[tri.a].z, 1.0f});
            vec4 v1 = mat4_mul_vec4(model, (vec4){mesh->vertices[tri.b].x, mesh->vertices[tri.b].y, mesh->vertices[tri.b].z, 1.0f});
            vec4 v2 = mat4_mul_vec4(model, (vec4){mesh->vertices[tri.c].x, mesh->vertices[tri.c].y, mesh->vertices[tri.c].z, 1.0f});

            vec3 p0 = {v0.x, v0.y, v0.z};
            vec3 p1 = {v1.x, v1.y, v1.z};
            vec3 p2 = {v2.x, v2.y, v2.z};

            //if (!CullTriangle(p0, p1, p2, camera)) continue;

			

            if (triCount >= triCapacity) {
                triCapacity = triCapacity == 0 ? 128 : triCapacity * 2;
                sortedTris = realloc(sortedTris, sizeof(SortedTriangle) * triCapacity);
            }

            if (ProjectTriangle(tri, mesh, mvp, width, height, &sortedTris[triCount])) {
                triCount++;
            }
        }
    }

    SortTriangles(sortedTris, triCount);
	
    if (WIREFRAME_MODE) {
		DrawTriangles(renderer, sortedTris, triCount);
	} else {
		for (int i = 0; i < triCount; ++i) {
			vec2 p0 = {sortedTris[i].x[0], sortedTris[i].y[0]};
			vec2 p1 = {sortedTris[i].x[1], sortedTris[i].y[1]};
			vec2 p2 = {sortedTris[i].x[2], sortedTris[i].y[2]};

			uint32_t color = 0xFFFFFFFF; // White
			RasterizeTriangleToBuffer(p0, p1, p2, color, pixels, pitch, width, height);
		}
	}
	
    free(sortedTris);

	// Unlock the texture now that all pixels are set
	SDL_UnlockTexture(frameBuffer);

	// Copy the whole framebuffer to the screen
	SDL_RenderClear(renderer);
	SDL_RenderTexture(renderer, frameBuffer, NULL, NULL);
	SDL_RenderPresent(renderer);
}


int LoadMeshFromFile(const char* filename, Mesh3D* mesh) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Failed to open file: %s\n", filename);
        return 0;
    }

    char line[512];  // Increased size to handle long face lines
    int vertexCount = 0, triangleCount = 0;
    vec3* vertices = NULL;
    Triangle* triangles = NULL;

    // First pass — count vertices & faces
    while (fgets(line, sizeof(line), file)) {
        if (line[0] == 'v' && line[1] == ' ') {
            vertexCount++;
            vertices = realloc(vertices, vertexCount * sizeof(vec3));
            float x, y, z;
            sscanf(line, "v %f %f %f", &x, &y, &z);
            vertices[vertexCount - 1] = (vec3){x, y, z};
        } 
        else if (line[0] == 'f' && line[1] == ' ') {
            int v[4], matches = 0;

            matches = sscanf(line, "f %d/%*d/%*d %d/%*d/%*d %d/%*d/%*d",
                             &v[0], &v[1], &v[2]);

            if (matches >= 3) {
                // Store the triangle
                triangleCount++;
                triangles = realloc(triangles, triangleCount * sizeof(Triangle));
                triangles[triangleCount - 1] = (Triangle){v[0] - 1, v[1] - 1, v[2] - 1};

                // If quad, split into two triangles
                if (matches == 4) {
                    triangleCount++;
                    triangles = realloc(triangles, triangleCount * sizeof(Triangle));
                    triangles[triangleCount - 1] = (Triangle){v[0] - 1, v[2] - 1, v[3] - 1};
                }
            }
        }
        // Ignore all other lines: "mtllib", "o", "vn", "vt", "usemtl", "s", etc.
    }

    fclose(file);

    // Fill mesh structure
    mesh->vertices = vertices;
    mesh->vertexCount = vertexCount;
    mesh->triangles = triangles;
    mesh->triangleCount = triangleCount;

    printf("Successfully Loaded: %s (%d vertices, %d triangles)\n", filename, vertexCount, triangleCount);
    return 1;
}


RenderData* defaultRendData() {
	RenderData* data = malloc(sizeof(RenderData));
	if (!data) return NULL;
	
	data->mesh = NULL;
	
	return data;
}

void RasterizeTriangleToBuffer(vec2 p0, vec2 p1, vec2 p2, uint32_t color, uint32_t* pixels, int pitch, int width, int height) {
    int minX = fmax(0, fmin(fmin(p0.x, p1.x), p2.x));
    int maxX = fmin(width - 1, fmax(fmax(p0.x, p1.x), p2.x));
    int minY = fmax(0, fmin(fmin(p0.y, p1.y), p2.y));
    int maxY = fmin(height - 1, fmax(fmax(p0.y, p1.y), p2.y));

    for (int y = minY; y <= maxY; y++) {
        for (int x = minX; x <= maxX; x++) {
            vec2 p = {x + 0.5f, y + 0.5f};

            float w0 = (p.x - p1.x) * (p2.y - p1.y) - (p.y - p1.y) * (p2.x - p1.x);
            float w1 = (p.x - p2.x) * (p0.y - p2.y) - (p.y - p2.y) * (p0.x - p2.x);
            float w2 = (p.x - p0.x) * (p1.y - p0.y) - (p.y - p0.y) * (p1.x - p0.x);

            if (w0 >= 0 && w1 >= 0 && w2 >= 0) {
                pixels[y * (pitch / 4) + x] = color;
            }
        }
    }
}

mat4 BuildView(Object* camera) {
	mat4 view = mat4_identity();
    view = mat4_mul(translate(vec3_negation(camera->transform.position)), view);
    view = mat4_mul(rotateZ(-camera->transform.rotation.z), view);
    view = mat4_mul(rotateY(-camera->transform.rotation.y), view);
    view = mat4_mul(rotateX(-camera->transform.rotation.x), view);
	return view;
}

mat4 BuildModel(Object* object) {
	mat4 model = mat4_identity();
    model = mat4_mul(translate(object->transform.position), model);
    model = mat4_mul(rotateZ(object->transform.rotation.z), model);
    model = mat4_mul(rotateY(object->transform.rotation.y), model);
    model = mat4_mul(rotateX(object->transform.rotation.x), model);
    model = mat4_mul(scale(object->transform.scale), model);
	return model;
}

int CullTriangle(vec3 p0, vec3 p1, vec3 p2, Object* camera) {
	mat4 view = BuildView(camera);
	vec4 v0 = mat4_mul_vec4(view, (vec4){p0.x, p0.y, p0.z, 1.0f});
    vec4 v1 = mat4_mul_vec4(view, (vec4){p1.x, p1.y, p1.z, 1.0f});
    vec4 v2 = mat4_mul_vec4(view, (vec4){p2.x, p2.y, p2.z, 1.0f});
	
	vec3 g0 = {v0.x, v0.y, v0.z};
    vec3 g1 = {v1.x, v1.y, v1.z};
    vec3 g2 = {v2.x, v2.y, v2.z};
	
    vec3 edge1 = vec3_add(g1, vec3_negation(g0));
    vec3 edge2 = vec3_add(g2, vec3_negation(g0));
    vec3 normal = vec3_cross(edge2, edge1);
    normal = vec3_normalize(normal);

    vec3 viewDir = vec3_negation(g0);
    viewDir = vec3_normalize(viewDir);

    return vec3_dot(normal, viewDir) < 0.0f;
}

int ProjectTriangle(Triangle tri, Mesh3D* mesh, mat4 mvp, int width, int height, SortedTriangle* out) {
    int screenX[3], screenY[3];
    float zDepth[3];

    for (int j = 0; j < 3; ++j) {
        int idx = (j == 0) ? tri.a : (j == 1) ? tri.b : tri.c;
        vec3 v = mesh->vertices[idx];

        vec4 vert = {v.x, v.y, v.z, 1.0f};
        vert = mat4_mul_vec4(mvp, vert);

        if (vert.w <= 0.0f) return 0; // Invalid projection

        vert.x /= vert.w;
        vert.y /= vert.w;
        vert.z /= vert.w;

        out->x[j] = (int)((vert.x * 0.5f + 0.5f) * width);
        out->y[j] = (int)((-vert.y * 0.5f + 0.5f) * height);
        zDepth[j] = vert.z;
    }

    out->depth = (zDepth[0] + zDepth[1] + zDepth[2]) / 3.0f;
    return 1; // Success
}

void SortTriangles(SortedTriangle* tris, int count) {
    int compareDepth(const void* a, const void* b) {
        float zA = ((SortedTriangle*)a)->depth;
        float zB = ((SortedTriangle*)b)->depth;
        return (zB > zA) - (zB < zA);
    }
    qsort(tris, count, sizeof(SortedTriangle), compareDepth);
}

void DrawTriangles(SDL_Renderer* renderer, SortedTriangle* tris, int count) {
    for (int i = 0; i < count; ++i) {
		
		SDL_RenderLine(renderer, 
            tris[i].x[0], tris[i].y[0],
            tris[i].x[1], tris[i].y[1]);
        SDL_RenderLine(renderer,
            tris[i].x[1], tris[i].y[1],
            tris[i].x[2], tris[i].y[2]);
        SDL_RenderLine(renderer,
            tris[i].x[2], tris[i].y[2],
            tris[i].x[0], tris[i].y[0]);
    }
}

mat4 BuildProjection(float fovDeg, float width, float height) {
    float aspect = width / height;
    return perspective(fovDeg, aspect, 0.1f, 1000.0f);
}