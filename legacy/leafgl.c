#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <ctype.h>

#include "leafgl.h"
#include "model.h"

//
// leafgl.c
//

// Forward declaration of the model's memory free function.
// This is needed because leafgl.c calls it, but the function is defined in model.c.
void free_struct_mem(Polygon3d* model);

Vect2 project(Vertex v, float x_pos, float y_pos, float z_pos) {
    Vect2 p;
    float fov = 1.0f;
    float z_factor = fov / (v.z + z_pos);

    // Basic projection
    p.x =(int)((v.x + x_pos) * z_factor * 200.0f + SCREEN_WIDTH / 2.0f);
    p.y =(int)(-(v.y + y_pos) * z_factor * 200.0f + SCREEN_HEIGHT / 2.0f);
    return p;
}

Vertex rotateY(Vertex v, float angle) {
    Vertex rotated;
    rotated.x = v.x * cosf(angle) - v.z * sinf(angle);
    rotated.y = v.y;
    rotated.z = v.x * sinf(angle) + v.z * cosf(angle);
    return rotated;
}

Vertex rotateX(Vertex v, float angle) {
    Vertex rotated;
    rotated.x = v.x;
    rotated.y = v.y * cosf(angle) - v.z * sinf(angle);
    rotated.z = v.y * sinf(angle) + v.z * cosf(angle);
    return rotated;
}

Vertex rotateZ(Vertex v, float angle) {
    Vertex rotated;
    rotated.x = v.x * cosf(angle) - v.y * sinf(angle);
    rotated.y = v.x * sinf(angle) + v.y * cosf(angle);
    rotated.z = v.z;
    return rotated;
}

void glCleanup(SDL_Renderer* renderer, SDL_Window* window, Polygon3d** models, int num_models) {
    if (models) {
        for (int i = 0; i < num_models; i++) {
            free_struct_mem(models[i]);
        }
        free(models); // Free the array itself
    }
    if (renderer) {
        SDL_DestroyRenderer(renderer);
    }
    if (window) {
        SDL_DestroyWindow(window);
    }
    SDL_Quit();
}

int glRenderWinInit() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("[leafgl/__main__::glRenderWinInit] SDL init error: %s\n", SDL_GetError());
        return -1;
    }
    return 1;
}

SDL_Window* glWindowInit(const char* window_name) {
    SDL_Window* window = NULL;
    window = SDL_CreateWindow(window_name, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        printf("[leafgl/__main__::glWindowInit] LeafGL SDL Window could not be set up. {desc: %s}\n", SDL_GetError());
        return NULL;
    }
    return window;
}

SDL_Renderer* glRendererInit(SDL_Window* window) {
    SDL_Renderer* renderer = NULL;
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (renderer == NULL) {
        printf("[leafgl/__main__::glRendererInit] LeafGL SDL Renderer could not be set up. {desc: %s}\n", SDL_GetError());
        return NULL;
    }
    return renderer;
}

Polygon3d* glModel3DInit(const char* model_file_name) {
    Polygon3d* model = read_model(model_file_name);
    if (model == NULL) {
        printf("[leafgl/__main__::glModel3DInit] Model '%s' could not be loaded.\n", model_file_name);
        return NULL;
    }
    return model;
}

void gldrawScene(SDL_Renderer* renderer, Polygon3d** models, int num_models, char rotation_axis, float rotation_angle, float x_pos, float y_pos, float z_pos) {
    // Clear the renderer once per frame
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderClear(renderer);

    // Loop through each model in the array
    for (int model_idx = 0; model_idx < num_models; model_idx++) {
        Polygon3d* current_model = models[model_idx];
        if (current_model == NULL) {
            continue; // Skip any un-initialized models
        }

        // forward initialization of variables to get them in scope.
        Vertex v1;
        Vertex v2;
        Vertex v3;

        // Loop through each face of the current model
        for (int i = 0; i < current_model->num_faces; i++) {
            Face face = current_model->faces[i];
            // Apply rotation to the vertices before projecting

            printf("debug log: rotation_axis is '%c'.\n",tolower(rotation_axis));

            if (tolower(rotation_axis) == 'y') {
                v1 = rotateY(current_model->vertices[face.v_idx[0]], rotation_angle);
                v2 = rotateY(current_model->vertices[face.v_idx[1]], rotation_angle);
                v3 = rotateY(current_model->vertices[face.v_idx[2]], rotation_angle);
            }

            if (tolower(rotation_axis) == 'x') {
                Vertex v1 = rotateX(current_model->vertices[face.v_idx[0]], rotation_angle);
                Vertex v2 = rotateX(current_model->vertices[face.v_idx[1]], rotation_angle);
                Vertex v3 = rotateX(current_model->vertices[face.v_idx[2]], rotation_angle);
            }

            if (tolower(rotation_axis) == 'z') {
                v1 = rotateZ(current_model->vertices[face.v_idx[0]], rotation_angle);
                v2 = rotateZ(current_model->vertices[face.v_idx[1]], rotation_angle);
                v3 = rotateZ(current_model->vertices[face.v_idx[2]], rotation_angle);
            }
            else {
                printf("[leafgl/__main__::gldrawScene] rotation_axis value was unmatched. {desc: use either x,y,z}\n");
                exit(EXIT_FAILURE);
            }

            // Project the 3D vertices to 2D screen coordinates
            // You might want to make x_pos, y_pos, and z_pos specific to each model
            Vect2 p1 = project(v1, x_pos, y_pos, z_pos);
            Vect2 p2 = project(v2, x_pos, y_pos, z_pos);
            Vect2 p3 = project(v3, x_pos, y_pos, z_pos);

            // Set color based on depth for a simple 3D feel
            Uint8 depth_color = (Uint8)(fmaxf(0.0f, fminf(255.0f, 255.0f * (1.0f - (v1.z + 5.0f) / 10.0f))));
            SDL_SetRenderDrawColor(renderer, depth_color, depth_color, 255 - depth_color, 0xFF);

            // Draw the wireframe triangle
            SDL_RenderDrawLine(renderer, p1.x, p1.y, p2.x, p2.y);
            SDL_RenderDrawLine(renderer, p2.x, p2.y, p3.x, p3.y);
            SDL_RenderDrawLine(renderer, p3.x, p3.y, p1.x, p1.y);
        }
    }

    // Present the renderer once after all models are drawn
    SDL_RenderPresent(renderer);
}

int glMainRenderLoop(SDL_Renderer* renderer, SDL_Window* window, Polygon3d** models, float x_pos, float y_pos, float z_pos, float rotation_axis, float rotation_angle, int num_models, bool beauty_turn) {
    SDL_Event e;
    int quit = 0;

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = 1;
            }
        }
        
        // Call the new scene-drawing function
        gldrawScene(renderer, models, num_models, rotation_axis, rotation_angle, x_pos, y_pos, z_pos);
        
        if (beauty_turn) {
            rotation_angle += 0.01f;
        }
        
        SDL_Delay(10);
    }

    glCleanup(renderer, window, models, num_models);

    return 0;
}
