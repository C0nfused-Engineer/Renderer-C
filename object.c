#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include "renderer.h"
#include "object.h"
#include "matrix.h"

Transform defaultTransform() { //Returns a default transform of (0,0,0), (1,1,1), (0,0,0)
	return (Transform){
		(vec3){0.0f, 0.0f, 0.0f}, 
		(vec3){1.0f, 1.0f, 1.0f}, 
		(vec3){0.0f, 0.0f, 0.0f}
	};
}

Object* createObject() {
	Object* newObject = malloc(sizeof(Object));
	newObject->transform = defaultTransform();
	//newObject.subobjects = NULL;
	newObject->isRendered = false;
	newObject->rendData = NULL;
	//newObject.isPhysics = false;
	//newObject.rendData = NULL;
	
	return newObject;
}

void setRenderData(Object* object, RenderData* rData) {
	object->isRendered = true;
	object->rendData = rData;
}

void hideObject(Object* object) {
	object->isRendered = false;
}

void unhideObject(Object* object) {
	object->isRendered = true;
}

void setTransformPosition(Object* object, vec3 newPos) {
	object->transform.position = newPos;
}

//void setPhysicsData(Object* object, PhysicsData* pData) {}