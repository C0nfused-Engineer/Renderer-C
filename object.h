#ifndef OBJECT_H
#define OBJECT_H

#include <stddef.h>
#include <stdlib.h>
#include "vector.h"
#include "renderer.h"
#include "matrix.h"

typedef struct RenderData RenderData;

typedef struct Transform {
	vec3 position;
	vec3 scale;
	vec3 rotation;
} Transform;

typedef struct Object {
	//Object Component
	Transform transform;
	//Object* subobjects;
	
	//Rendering Component
	bool isRendered;
	RenderData* rendData;
	
	//Physics Component
	//bool isPhysics;
	//PhysicsData* physData;
} Object;

//General Object Functions
Object* createObject();
Transform defaultTransform();

//Object RenderData Functions
void setRenderData(Object* object, RenderData* rData);
void hideObject(Object* object);
void unhideObject(Object* object);
void setTransformPosition(Object* object, vec3 newPos);

//Object Physics Data Functions
//void setPhysicsData(Object* object, PhysicsData* pData);
#endif