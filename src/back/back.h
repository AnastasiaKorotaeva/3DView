#ifndef BACK_H

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Matrix {
  double **matrix;
  int rows;
  int cols;
} matrix_t;

typedef struct Facets {
  int *vertices;
  int count_vertices_in_facets;
} polygon_t;

typedef struct Data {
  int count_of_vertices;
  int count_of_facets;
  matrix_t matrix_3d;
  polygon_t *polygons;
  double rotation_x;
  double rotation_y;
  double rotation_z;
} model_t;

void CountVerticesAndFacets(FILE *file, model_t *model);
int AllocateMemory(model_t *model);
int ParseModelData(FILE *file, model_t *model);
void RotateModel(model_t *model, double step, char xyz);
void RotatePoint(double *point, double angle, char xyz);
void ApplyRotation(model_t *model);
void MoveModel(model_t *model, double distance, char xyz);
void FreeMemory(model_t *model);

#endif  // BACK_H
