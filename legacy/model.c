#include <string.h>

#include "model.h"

// 
// model.c
//

void free_struct_mem(Polygon3d* model) {
    if (model) {
        free(model->vertices);
        free(model->texcoords);
        free(model->normals);
        free(model->faces);
        free(model);
    }
}

Polygon3d* read_model(const char* fileName) {
    if (strstr(fileName, ".obj")) {
        FILE* file = fopen(fileName, "r");
        if (!file) {
            printf("[leafgl/model.c::read_model] file '%s' was type null.\n", fileName);
            return NULL;
        }

        Polygon3d* model = (Polygon3d*)malloc(sizeof(Polygon3d));
        if (!model) {
            printf("[leafgl/model.c::read_model] unable to allocate memory for _Polygon3d_ struct.\n");
            fclose(file);
            return NULL;
        }

        model->num_vertices = 0;
        model->num_texcoords = 0;
        model->num_normals = 0;
        model->num_faces = 0;

        int vertex_capacity = 100;
        int texcoord_capacity = 100;
        int normal_capacity = 100;
        int face_capacity = 100;

        model->vertices = (Vertex*)malloc(vertex_capacity * sizeof(Vertex));
        model->texcoords = (TexCoord*)malloc(texcoord_capacity * sizeof(TexCoord));
        model->normals = (Normal*)malloc(normal_capacity * sizeof(Normal));
        model->faces = (Face*)malloc(face_capacity * sizeof(Face));

        char line[256];

        while (fgets(line, sizeof(line), file)) {
            if (line[0] == 'v' && line[1] == ' ') {
                if (model->num_vertices >= vertex_capacity) {
                    vertex_capacity *= 2;
                    void* temp = realloc(model->vertices, vertex_capacity * sizeof(Vertex));
                    if (temp == NULL) { free_struct_mem(model); fclose(file); return NULL; }
                    model->vertices = (Vertex*)temp;
                }
                sscanf(line, "v %f %f %f", &model->vertices[model->num_vertices].x, &model->vertices[model->num_vertices].y, &model->vertices[model->num_vertices].z);
                model->num_vertices++;
            } else if (line[0] == 'v' && line[1] == 't') {
                if (model->num_texcoords >= texcoord_capacity) {
                    texcoord_capacity *= 2;
                    void* temp = realloc(model->texcoords, texcoord_capacity * sizeof(TexCoord));
                    if (temp == NULL) { free_struct_mem(model); fclose(file); return NULL; }
                    model->texcoords = (TexCoord*)temp;
                }
                sscanf(line, "vt %f %f", &model->texcoords[model->num_texcoords].u, &model->texcoords[model->num_texcoords].v);
                model->num_texcoords++;
            } else if (line[0] == 'v' && line[1] == 'n') {
                if (model->num_normals >= normal_capacity) {
                    normal_capacity *= 2;
                    void* temp = realloc(model->normals, normal_capacity * sizeof(Normal));
                    if (temp == NULL) { free_struct_mem(model); fclose(file); return NULL; }
                    model->normals = (Normal*)temp;
                }
                sscanf(line, "vn %f %f %f", &model->normals[model->num_normals].x, &model->normals[model->num_normals].y, &model->normals[model->num_normals].z);
                model->num_normals++;
            } else if (line[0] == 'f') {
                if (model->num_faces >= face_capacity) {
                    face_capacity *= 2;
                    void* temp = realloc(model->faces, face_capacity * sizeof(Face));
                    if (temp == NULL) { free_struct_mem(model); fclose(file); return NULL; }
                    model->faces = (Face*)temp;
                }
                int v1, vt1, vn1, v2, vt2, vn2, v3, vt3, vn3;
                sscanf(line, "f %d/%d/%d %d/%d/%d %d/%d/%d", &v1, &vt1, &vn1, &v2, &vt2, &vn2, &v3, &vt3, &vn3);
                model->faces[model->num_faces].v_idx[0] = v1 - 1;
                model->faces[model->num_faces].vt_idx[0] = vt1 - 1;
                model->faces[model->num_faces].vn_idx[0] = vn1 - 1;
                model->faces[model->num_faces].v_idx[1] = v2 - 1;
                model->faces[model->num_faces].vt_idx[1] = vt2 - 1;
                model->faces[model->num_faces].vn_idx[1] = vn2 - 1;
                model->faces[model->num_faces].v_idx[2] = v3 - 1;
                model->faces[model->num_faces].vt_idx[2] = vt3 - 1;
                model->faces[model->num_faces].vn_idx[2] = vn3 - 1;
                model->num_faces++;
            }
        }
        fclose(file);
        return model;
    } else {
        printf("[leafgl/model.c::read_model] model file is not an object. please consider renaming the file.\n");
        return NULL;
    }
}
