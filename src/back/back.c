#include "back.h"

void CountVerticesAndFacets(FILE *file, model_t *model) {
  char line[256];
  (*model).count_of_vertices = 0;
  (*model).count_of_facets = 0;
  while (fgets(line, sizeof(line), file) != NULL) {
    if (line[0] == 'v' && line[1] == ' ') {
      (*model).count_of_vertices++;
    } else if (line[0] == 'f' && line[1] == ' ') {
      char *token = strtok(line + 2, " ");
      int count_vertices_in_facets = 0;
      while (token != NULL) {
        int current_vertex_index = 0;
        if (sscanf(token, "%d", &current_vertex_index) == 1) {
          count_vertices_in_facets++;
        }
        token = strtok(NULL, " ");
      }
      if (count_vertices_in_facets == 2) {
        (*model).count_of_facets += count_vertices_in_facets - 1;
      } else if (count_vertices_in_facets > 2) {
        (*model).count_of_facets += count_vertices_in_facets - 2;
      }
    }
  }
}

int AllocateMemory(model_t *model) {
  int ERROR = 1;
  (*model).matrix_3d.rows = (*model).count_of_vertices + 1;
  (*model).matrix_3d.cols = 3;
  (*model).matrix_3d.matrix =
      (double **)calloc((*model).matrix_3d.rows, sizeof(double *));
  if ((*model).matrix_3d.matrix == NULL) {
    ERROR = 0;
  } else {
    for (int i = 1; i <= (*model).count_of_vertices && ERROR; i++) {
      (*model).matrix_3d.matrix[i] = (double *)calloc(3, sizeof(double));
      if ((*model).matrix_3d.matrix[i] == NULL) {
        ERROR = 0;
      }
    }
    (*model).polygons =
        (polygon_t *)calloc(((*model).count_of_facets + 1), sizeof(polygon_t));
    if ((*model).polygons == NULL) {
      ERROR = 0;
    }
  }
  return ERROR;
}

int ParseModelData(FILE *file, model_t *model) {
  int ERROR = 1, vertex_index = 1, facet_index = 1, count_vertex = 0;
  char line[256];
  double x, y, z = 0;
  if (!AllocateMemory(model))
    ERROR = 0;
  else {
    fseek(file, 0, SEEK_SET);
    while (fgets(line, sizeof(line), file) != NULL && ERROR) {
      if (line[0] == 'v' && line[1] == ' ') {
        if (sscanf(line, "v %lf %lf %lf", &x, &y, &z) == 3) {
          (*model).matrix_3d.matrix[vertex_index][0] = x;
          (*model).matrix_3d.matrix[vertex_index][1] = y;
          (*model).matrix_3d.matrix[vertex_index][2] = z;
          vertex_index++;
        } else
          ERROR = 0;
        count_vertex++;
      }
      if (line[0] == 'f' && line[1] == ' ') {
        char *token = strtok(line + 2, " ");
        int count_vertex_in_facets = 0;
        while (token != NULL && ERROR) {
          int current_vertex_index = 0;
          if (sscanf(token, "%d", &current_vertex_index) == 1) {
            if (current_vertex_index < 0)
              current_vertex_index = count_vertex + 1 + current_vertex_index;
            if (current_vertex_index == 0 ||
                current_vertex_index > vertex_index)
              ERROR = 0;
            (*model).polygons[facet_index].vertices =
                (int *)realloc((*model).polygons[facet_index].vertices,
                               (count_vertex_in_facets + 1) * sizeof(int));
            if (((*model).polygons[facet_index].vertices) == NULL) {
              ERROR = 0;
            } else
              (*model).polygons[facet_index].vertices[count_vertex_in_facets] =
                  current_vertex_index;
            count_vertex_in_facets++;
          }
          token = strtok(NULL, " ");
        }
        (*model).polygons[facet_index].count_vertices_in_facets =
            count_vertex_in_facets;
        if (count_vertex_in_facets > 1) facet_index++;
      }
    }
  }
  if (ERROR == 0) FreeMemory(model);
  return ERROR;
}

void FreeMemory(model_t *model) {
  if ((*model).matrix_3d.matrix != NULL) {
    for (int i = 0; i <= (*model).count_of_vertices; i++) {
      free((*model).matrix_3d.matrix[i]);
    }
    free((*model).matrix_3d.matrix);
  }
  if ((*model).polygons != NULL) {
    for (int i = 0; i <= (*model).count_of_facets; i++) {
      free((*model).polygons[i].vertices);
    }
    free((*model).polygons);
  }
  (*model).count_of_vertices = 0;
  (*model).count_of_facets = 0;
}

void RotateModel(model_t *model, double step, char xyz) {
  switch (xyz) {
    case 'x':
      (*model).rotation_x = step;
      break;
    case 'y':
      (*model).rotation_y = step;
      break;
    case 'z':
      (*model).rotation_z = step;
      break;
    case 'X':
      (*model).rotation_x = -step;
      break;
    case 'Y':
      (*model).rotation_y = -step;
      break;
    case 'Z':
      (*model).rotation_z = -step;
      break;
    default:
      break;
  }
}

void RotatePoint(double *point, double angle, char xyz) {
  double cos_result = cos(angle);
  double sin_result = sin(angle);
  double temp = 0;
  switch (xyz) {
    case 'x':
      temp = point[1] * cos_result - point[2] * sin_result;
      point[2] = point[1] * sin_result + point[2] * cos_result;
      point[1] = temp;
      break;
    case 'y':
      temp = point[0] * cos_result + point[2] * sin_result;
      point[2] = -point[0] * sin_result + point[2] * cos_result;
      point[0] = temp;
      break;
    case 'z':
      temp = point[0] * cos_result - point[1] * sin_result;
      point[1] = point[0] * sin_result + point[1] * cos_result;
      point[0] = temp;
      break;
  }
}

void ApplyRotation(model_t *model) {
  for (int i = 1; i <= (*model).count_of_vertices; i++) {
    RotatePoint((*model).matrix_3d.matrix[i], (*model).rotation_x, 'x');
    RotatePoint((*model).matrix_3d.matrix[i], (*model).rotation_y, 'y');
    RotatePoint((*model).matrix_3d.matrix[i], (*model).rotation_z, 'z');
  }
  (*model).rotation_x = 0.0;
  (*model).rotation_y = 0.0;
  (*model).rotation_z = 0.0;
}

void MoveModel(model_t *model, double distance, char xyz) {
  switch (xyz) {
    case 'x':
      for (int i = 1; i <= (*model).count_of_vertices; i++) {
        (*model).matrix_3d.matrix[i][0] += distance;
      }
      break;
    case 'y':
      for (int i = 1; i <= (*model).count_of_vertices; i++) {
        (*model).matrix_3d.matrix[i][1] += distance;
      }
      break;
    case 'z':
      for (int i = 1; i <= (*model).count_of_vertices; i++) {
        (*model).matrix_3d.matrix[i][2] += distance;
      }
      break;
    case 'X':
      for (int i = 1; i <= (*model).count_of_vertices; i++) {
        (*model).matrix_3d.matrix[i][0] -= distance;
      }
      break;
    case 'Y':
      for (int i = 1; i <= (*model).count_of_vertices; i++) {
        (*model).matrix_3d.matrix[i][1] -= distance;
      }
      break;
    case 'Z':
      for (int i = 1; i <= (*model).count_of_vertices; i++) {
        (*model).matrix_3d.matrix[i][2] -= distance;
      }
      break;
    default:
      break;
  }
}
