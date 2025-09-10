#define LEAFGL_IMPL
#define LEAFGL_MODEL_IMPL
#include "../leafgl.h"

#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>

typedef struct {
    float x, y, z, rotation;
} GameObject;

int main(int argc, char* argv[]) {
    GameObject gameobject = {-1.0f, 0.0f, 5.0f, 0.0f};

    Polygon3d* cube_model = NULL;
    Polygon3d** models_array = NULL;
    int num_models = 0;

    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;

    printf("[example.c] starting renderer...\n");

    if (glRenderWinInit() < 0) {
        printf("[example.c] Failed to initialize SDL.\n");
        return 1;
    }

    window = glWindowInit("LeafGL Example");
    if (!window) {
        printf("[example.c] Failed to create window.\n");
        glCleanup(renderer, window, NULL, 0);
        return 1;
    }

    renderer = glRendererInit(window);
    if (!renderer) {
        printf("[example.c] Failed to create renderer.\n");
        glCleanup(renderer, window, NULL, 0);
        return 1;
    }

    cube_model = glModel3DInit("../example_models/portalgun_lowpoly.obj");
    if (!cube_model) {
        printf("[example.c] Could not load model 'portalgun_lowpoly.obj'.\n");
        glCleanup(renderer, window, NULL, 0);
        return 1;
    }

    num_models = 1;
    models_array = (Polygon3d**)malloc(sizeof(Polygon3d*) * num_models);
    if (!models_array) {
        printf("[example.c] Failed to allocate memory for models array.\n");
        glCleanup(renderer, window, &cube_model, 1);
        return 1;
    }
    models_array[0] = cube_model;

    // Main loop
    int running = 1;
    SDL_Event event;
    const Uint8* keystate;
    printf("Controls -> W is to increase z, S is to decrease z, A is to decrease x, D is to increase x, z & x is to change y, and t & y control rotation\n");
    while (running) {
        // Poll SDL events
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }
        }

        // Get current key state
        keystate = SDL_GetKeyboardState(NULL);

        // 'Movement'
        if (keystate[SDL_SCANCODE_W]) gameobject.z += 0.2f;
        if (keystate[SDL_SCANCODE_S]) gameobject.z -= 0.2f;
        if (keystate[SDL_SCANCODE_A]) gameobject.x -= 0.2f;
        if (keystate[SDL_SCANCODE_D]) gameobject.x += 0.2f;
        if (keystate[SDL_SCANCODE_Z]) gameobject.y += 0.2f;
        if (keystate[SDL_SCANCODE_X]) gameobject.y -= 0.2f;
        if (keystate[SDL_SCANCODE_T]) gameobject.rotation -= 0.2f;
        if (keystate[SDL_SCANCODE_Y]) gameobject.rotation += 0.2f;

        // Quit key
        if (keystate[SDL_SCANCODE_Q]) running = 0;

        // Optional: slow rotation
        // gameobject.rotation += 0.01f;

        // Render scene
        gldrawScene(renderer, models_array, num_models, 'x',
                    gameobject.rotation,
                    gameobject.x, gameobject.y, gameobject.z);

        SDL_Delay(10);
	// frame delay
    }

    glCleanup(renderer, window, models_array, num_models);
    return 0;
}
