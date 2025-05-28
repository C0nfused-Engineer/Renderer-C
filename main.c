#include <SDL3/SDL.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "vector.h"
#include "queue.h"
#include "object.h"
#include "matrix.h"

#include "renderer.h"

int windowWidth = 1920;
int windowHeight = 1080;

Queue renderQueue = {0};

vec3 cameraPos = {0.0f, 0.0f, -3.0f};
float FOV = 60.0f;

_Bool mouseLocked = true;

int main(void) {
	
	Mesh3D cubeMesh;
	LoadMeshFromFile("blenderCube.obj", &cubeMesh);
	
	Mesh3D csmMesh;
	LoadMeshFromFile("ApolloCSM.obj", &csmMesh);

	RenderData cubeModel = {&cubeMesh};
	RenderData csmModel = {&csmMesh};
	
	Object* cameraObject = createObject();
	setTransformPosition(cameraObject, cameraPos);
	
	Object* cubeA = createObject();
	setRenderData(cubeA, &cubeModel);
	setTransformPosition(cubeA, (vec3){0.0, 0.0, 0.0});
	
	Object* cubeB = createObject();
	setRenderData(cubeB, &cubeModel);
	setTransformPosition(cubeB, (vec3){0.0, 0.0, 8.0});
	
	Object* cubeC = createObject();
	setRenderData(cubeC, &cubeModel);
	setTransformPosition(cubeC, (vec3){0.0, 0.0, -8.0});
	
	Object* CSM = createObject();
	setRenderData(CSM, &csmModel);
	setTransformPosition(CSM, (vec3){0.0, 10.0, 0.0});
	
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) == false) {
        SDL_Log("SDL could not initialize! SDL_Error: %s", SDL_GetError());
        return 1;
    }

    // Create the window
    SDL_Window* window = SDL_CreateWindow("C Renderer", windowWidth, windowHeight, 0);
    if (!window) {
        SDL_Log("Window could not be created! SDL_Error: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Create the renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, 0);
    if (!renderer) {
        SDL_Log("Renderer could not be created! SDL_Error: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

	SDL_Texture* frameBuffer = SDL_CreateTexture(
		renderer,
		SDL_PIXELFORMAT_ARGB8888,
		SDL_TEXTUREACCESS_STREAMING,
		windowWidth,
		windowHeight
	);

	SDL_SetWindowRelativeMouseMode(window, true); // Start with mouse locked

    // Set the background color to black
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Main loop
    SDL_Event e;
    int running = 1;
    while (running) {
		
		enqueue(&renderQueue, cubeA);
		enqueue(&renderQueue, cubeB);
		enqueue(&renderQueue, cubeC);
		//enqueue(&renderQueue, CSM);
		
        // Poll for events
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_EVENT_QUIT) {  // Fixed event type
                running = 0;
            }
		
			handleCameraMouseLook(&e, cameraObject, &mouseLocked, window);
        }

		handleCameraMovement(cameraObject);

		RenderPass(renderer, frameBuffer, &renderQueue, cameraObject, FOV, windowWidth, windowHeight);

        // Delay to limit the frame rate
        SDL_Delay(16); // ~60 FPS
    }

    // Cleanup
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

// Called INSIDE event polling loop:
void handleCameraMouseLook(SDL_Event *e, Object* camera, _Bool *mouseLocked, SDL_Window* window) {
    if (e->type == SDL_EVENT_KEY_DOWN && e->key.scancode == SDL_SCANCODE_TAB) {
        *mouseLocked = !(*mouseLocked);
        SDL_SetWindowRelativeMouseMode(window, *mouseLocked);
    }

    if (*mouseLocked && e->type == SDL_EVENT_MOUSE_MOTION) {
        float sensitivity = 0.002f;

        camera->transform.rotation.y -= e->motion.xrel * sensitivity;
        camera->transform.rotation.x -= e->motion.yrel * sensitivity;

        if (camera->transform.rotation.x > M_PI / 2.0f) camera->transform.rotation.x = M_PI / 2.0f;
        if (camera->transform.rotation.x < -M_PI / 2.0f) camera->transform.rotation.x = -M_PI / 2.0f;
    }
}

// Called OUTSIDE event polling, every frame:
void handleCameraMovement(Object* camera) {
    const _Bool* state = SDL_GetKeyboardState(NULL);
    float speed = 0.05f;

    float yaw = camera->transform.rotation.y;

    vec3 forward = {
        sinf(yaw),
        0.0f,
        cosf(yaw)
    };

    vec3 right = {
        cosf(yaw),
        0.0f,
        -sinf(yaw)
    };

    if (state[SDL_SCANCODE_W]) {
        camera->transform.position.x -= forward.x * speed;
        camera->transform.position.z -= forward.z * speed;
    }
    if (state[SDL_SCANCODE_S]) {
        camera->transform.position.x += forward.x * speed;
        camera->transform.position.z += forward.z * speed;
    }
    if (state[SDL_SCANCODE_A]) {
        camera->transform.position.x -= right.x * speed;
        camera->transform.position.z -= right.z * speed;
    }
    if (state[SDL_SCANCODE_D]) {
        camera->transform.position.x += right.x * speed;
        camera->transform.position.z += right.z * speed;
    }
    if (state[SDL_SCANCODE_SPACE]) {
        camera->transform.position.y += speed;
    }
    if (state[SDL_SCANCODE_LCTRL]) {
        camera->transform.position.y -= speed;
    }
}
