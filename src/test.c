#include <check.h>

#include "back/back.h"

START_TEST(fun_s21_CountVerticesAndFacets) {
  model_t model;
  FILE *file = fopen("../obj/skull.obj", "r");
  CountVerticesAndFacets(file, &model);
  ck_assert_int_eq(model.count_of_vertices, 6122);
  ck_assert_int_eq(model.count_of_facets, 9537);
  fclose(file);
}
END_TEST

START_TEST(fun_s21_ParseModelData) {
  model_t model;
  FILE *file = fopen("../obj/skull.obj", "r");
  CountVerticesAndFacets(file, &model);
  int result = ParseModelData(file, &model);
  ck_assert_int_eq(result, 1);
  fclose(file);
  FreeMemory(&model);
}
END_TEST

START_TEST(fun_s21_ParseModelData_incorrect_1) {
  model_t model;
  FILE *file = fopen("../obj/vertex_less_than_3.obj", "r");
  CountVerticesAndFacets(file, &model);
  int result = ParseModelData(file, &model);
  ck_assert_int_eq(result, 0);
  fclose(file);
}
END_TEST

START_TEST(fun_s21_ParseModelData_incorrect_2) {
  model_t model;
  FILE *file = fopen("../obj/facet_that_less_than_1.obj", "r");
  CountVerticesAndFacets(file, &model);
  int result = ParseModelData(file, &model);
  ck_assert_int_eq(result, 0);
  fclose(file);
}
END_TEST

START_TEST(fun_s21_ParseModelData_incorrect_3) {
  model_t model;
  FILE *file = fopen("../obj/facet_that_not_exist.obj", "r");
  CountVerticesAndFacets(file, &model);
  int result = ParseModelData(file, &model);
  ck_assert_int_eq(result, 0);
  fclose(file);
}
END_TEST

START_TEST(fun_s21_RotateModel_1) {
  model_t model;
  FILE *file = fopen("../obj/cube.obj", "r");
  CountVerticesAndFacets(file, &model);
  ParseModelData(file, &model);
  fclose(file);

  RotateModel(&model, 0.0175, 'x');
  RotateModel(&model, 0.0175, 'y');
  RotateModel(&model, 0.0175, 'z');
  ApplyRotation(&model);

  ck_assert_double_eq_tol(model.rotation_x, 0.0, 1e-6);
  ck_assert_double_eq_tol(model.rotation_y, 0.0, 1e-6);
  ck_assert_double_eq_tol(model.rotation_z, 0.0, 1e-6);

  double expected_vertices[] = {
      0.687263,  -0.660442, 0.734875,  -0.312429, -0.677939, 0.752374,
      -0.330583, -0.639963, -0.246739, -0.347399, 0.359161,  -0.208458,
      -0.329246, 0.321186,  0.790655,  0.670447,  0.338683,  0.773156,
      0.652294,  0.376658,  -0.225957, 0.669110,  -0.622467, -0.264238};

  int i = 0;
  while (i < (int)(model.count_of_vertices * 3)) {
    ck_assert_double_eq_tol(model.matrix_3d.matrix[i / 3 + 1][i % 3],
                            expected_vertices[i], 1e-6);
    i++;
  }
  FreeMemory(&model);
}
END_TEST

START_TEST(fun_s21_RotateModel_2) {
  model_t model;
  FILE *file = fopen("../obj/cube.obj", "r");
  CountVerticesAndFacets(file, &model);
  ParseModelData(file, &model);
  fclose(file);

  RotateModel(&model, 0.0175, 'X');
  RotateModel(&model, 0.0175, 'Y');
  RotateModel(&model, 0.0175, 'Z');
  ApplyRotation(&model);

  ck_assert_double_eq_tol(model.rotation_x, 0.0, 1e-6);
  ck_assert_double_eq_tol(model.rotation_y, 0.0, 1e-6);
  ck_assert_double_eq_tol(model.rotation_z, 0.0, 1e-6);

  double expected_vertices[] = {
      0.637667,  -0.657090, 0.781133,  -0.362025, -0.639593, 0.763634,
      -0.344471, -0.636604, -0.236207, -0.327030, 0.363238,  -0.232911,
      -0.344584, 0.360248,  0.766930,  0.655109,  0.342752,  0.784429,
      0.672663,  0.345742,  -0.215412, 0.655221,  -0.654100, -0.218708};

  int i = 0;
  while (i < (int)(model.count_of_vertices * 3)) {
    ck_assert_double_eq_tol(model.matrix_3d.matrix[i / 3 + 1][i % 3],
                            expected_vertices[i], 1e-6);
    i++;
  }
  FreeMemory(&model);
}
END_TEST

START_TEST(fun_s21_RotateModel_3) {
  model_t model;
  FILE *file = fopen("../obj/cube.obj", "r");
  CountVerticesAndFacets(file, &model);
  ParseModelData(file, &model);
  fclose(file);
  RotateModel(&model, 0.0175, 'd');
  FreeMemory(&model);
}
END_TEST

START_TEST(fun_s21_TranslateModel_1) {
  model_t model;
  FILE *file = fopen("../obj/cube.obj", "r");
  CountVerticesAndFacets(file, &model);
  ParseModelData(file, &model);
  fclose(file);

  double translation_distance = 2.0;

  MoveModel(&model, translation_distance, 'x');
  MoveModel(&model, -translation_distance, 'y');
  MoveModel(&model, translation_distance, 'z');

  double expected_vertices[] = {
      2.662638, -2.659200, 2.758236, 1.662639, -2.659200, 2.758236,
      1.662639, -2.638405, 1.758453, 1.662639, -1.638622, 1.779248,
      1.662639, -1.659417, 2.779031, 2.662638, -1.659417, 2.779031,
      2.662638, -1.638622, 1.779248, 2.662638, -2.638405, 1.758453};

  int i = 0;
  while (i < (int)(model.count_of_vertices * 3)) {
    ck_assert_double_eq_tol(model.matrix_3d.matrix[i / 3 + 1][i % 3],
                            expected_vertices[i], 1e-6);
    i++;
  }
  FreeMemory(&model);
}
END_TEST

START_TEST(fun_s21_TranslateModel_2) {
  model_t model;
  FILE *file = fopen("../obj/cube.obj", "r");
  CountVerticesAndFacets(file, &model);
  ParseModelData(file, &model);
  fclose(file);

  double translation_distance = 2.0;

  MoveModel(&model, translation_distance, 'X');
  MoveModel(&model, translation_distance, 'Y');
  MoveModel(&model, translation_distance, 'Z');

  double expected_vertices[] = {
      -1.337362, -2.659200, -1.241764, -2.337361, -2.659200, -1.241764,
      -2.337361, -2.638405, -2.241547, -2.337361, -1.638622, -2.220752,
      -2.337361, -1.659417, -1.220969, -1.337362, -1.659417, -1.220969,
      -1.337362, -1.638622, -2.220752, -1.337362, -2.638405, -2.241547};

  int i = 0;
  while (i < (int)(model.count_of_vertices * 3)) {
    ck_assert_double_eq_tol(model.matrix_3d.matrix[i / 3 + 1][i % 3],
                            expected_vertices[i], 1e-6);
    i++;
  }
  FreeMemory(&model);
}
END_TEST

START_TEST(fun_s21_TranslateModel_3) {
  model_t model;
  FILE *file = fopen("../obj/cube.obj", "r");
  CountVerticesAndFacets(file, &model);
  ParseModelData(file, &model);
  fclose(file);

  double translation_distance = 2.0;

  MoveModel(&model, translation_distance, 't');
  FreeMemory(&model);
}
END_TEST

Suite *suite_fun(void) {
  Suite *suite;
  TCase *all_tcase;

  suite = suite_create("fun_3d_viewer");
  all_tcase = tcase_create("All_case");

  tcase_add_test(all_tcase, fun_s21_CountVerticesAndFacets);
  tcase_add_test(all_tcase, fun_s21_ParseModelData);
  tcase_add_test(all_tcase, fun_s21_ParseModelData_incorrect_1);
  tcase_add_test(all_tcase, fun_s21_ParseModelData_incorrect_2);
  tcase_add_test(all_tcase, fun_s21_ParseModelData_incorrect_3);
  tcase_add_test(all_tcase, fun_s21_RotateModel_1);
  tcase_add_test(all_tcase, fun_s21_RotateModel_2);
  tcase_add_test(all_tcase, fun_s21_RotateModel_3);
  tcase_add_test(all_tcase, fun_s21_TranslateModel_1);
  tcase_add_test(all_tcase, fun_s21_TranslateModel_2);
  tcase_add_test(all_tcase, fun_s21_TranslateModel_3);

  suite_add_tcase(suite, all_tcase);
  return suite;
}

int main(void) {
  int failed_count;
  Suite *suite;
  SRunner *srunner;

  suite = suite_fun();
  srunner = srunner_create(suite);

  srunner_set_fork_status(srunner, CK_NOFORK);
  srunner_run_all(srunner, CK_NORMAL);
  failed_count = srunner_ntests_failed(srunner);
  srunner_free(srunner);
  return (failed_count == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
