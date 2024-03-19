#include "openglwidget.h"

OpenGLWidget::OpenGLWidget(QWidget *parent)
    : QOpenGLWidget(parent),
      fileLoaded(false),
      scale(1.0),
      pointColor(Qt::red),
      lineColor(Qt::gray) {
  setFixedSize(winWidth, winHeight);
}

void OpenGLWidget::initializeGL() { initializeOpenGLFunctions(); }

void OpenGLWidget::resizeGL(int w, int h) { glViewport(0, 0, w, h); }

void OpenGLWidget::paintGL() {
  if (!fileLoaded) {
    return;
  }
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  if (useDottedLine) {
    glEnable(GL_LINE_STIPPLE);
  } else {
    glDisable(GL_LINE_STIPPLE);
  }
  if (useDottedVer == 1) {
    glEnable(GL_POINT_SMOOTH);
  } else {
    glDisable(GL_POINT_SMOOTH);
  }
  if (useDottedVer == 2) {
    glEnable(GL_TEXTURE_2D);
  } else {
    glDisable(GL_TEXTURE_2D);
  }
  if (useDottedVer != 0) {
    glColor3f(pointColor.redF(), pointColor.greenF(), pointColor.blueF());
    glBegin(GL_POINTS);
    for (int i = 1; i <= model.count_of_vertices; i++) {
      glVertex3dv(model.matrix_3d.matrix[i]);
    }
    glEnd();
  }

  glColor3f(lineColor.redF(), lineColor.greenF(), lineColor.blueF());
  glBegin(GL_LINES);
  for (int i = 1; i <= model.count_of_facets; i++) {
    int numVertices = model.polygons[i].count_vertices_in_facets;
    for (int j = 0; j < numVertices; j++) {
      int currentVertexIndex = model.polygons[i].vertices[j];
      int nextVertexIndex = model.polygons[i].vertices[(j + 1) % numVertices];
      if (currentVertexIndex < 0) {
        currentVertexIndex = model.count_of_vertices + 1 + currentVertexIndex;
      }
      if (nextVertexIndex < 0) {
        nextVertexIndex = model.count_of_vertices + 1 + nextVertexIndex;
      }
      glVertex3dv(model.matrix_3d.matrix[currentVertexIndex]);
      glVertex3dv(model.matrix_3d.matrix[nextVertexIndex]);
    }
  }
  glEnd();
  glDisable(GL_LINE_STIPPLE);
  emit countVertexFacets(model.count_of_vertices, model.count_of_facets);
}

void OpenGLWidget::loadModelFile(const QString &filePath) {
  FILE *file = fopen(filePath.toStdString().c_str(), "r");
  if (file) {
    CountVerticesAndFacets(file, &model);
    if (ParseModelData(file, &model) == 1) {
      fclose(file);
      centerModel();
      double scaleFactor = 1;
      scaleModelToFit(scaleFactor);
      update();
      fileLoaded = true;
    } else {
      fclose(file);
      update();
      emit fileIncorrect("File incorrect");
    }
  } else {
    emit fileIncorrect("Error opening file ");
  }
}

void OpenGLWidget::setProjectionType(int value) {
  makeCurrent();
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  if (value == 0) {
    // Центральная
    float fov = 60 * M_PI / 180;
    float heapHeight = winHeight / (2 * tan(fov / 2));
    glFrustum(-winWidth / 12, winWidth / 12, -winHeight / 12, winHeight / 12,
              heapHeight, 2);
  } else if (value == 1) {
    // Параллельная
    glOrtho(-winWidth / winWidth, winWidth / winWidth, -winHeight / winHeight,
            winHeight / winHeight, -100, 100);
  }
  glTranslatef(0.0, 0.0, -10.0);
  glMatrixMode(GL_MODELVIEW);
  doneCurrent();
  update();
}

void OpenGLWidget::centerModel() {
  if (model.count_of_vertices == 0) {
    return;
  }
  double centerX = 0.0;
  double centerY = 0.0;
  double centerZ = 0.0;

  for (int i = 1; i <= model.count_of_vertices; i++) {
    centerX += model.matrix_3d.matrix[i][0];
    centerY += model.matrix_3d.matrix[i][1];
    centerZ += model.matrix_3d.matrix[i][2];
  }

  centerX /= model.count_of_vertices;
  centerY /= model.count_of_vertices;
  centerZ /= model.count_of_vertices;

  for (int i = 1; i <= model.count_of_vertices; i++) {
    model.matrix_3d.matrix[i][0] -= centerX;
    model.matrix_3d.matrix[i][1] -= centerY;
    model.matrix_3d.matrix[i][2] -= centerZ;
  }
}

void OpenGLWidget::scaleModelToFit(double scaleFactor) {
  makeCurrent();
  if (model.count_of_vertices == 0) {
    return;
  }
  double maxDistance = 0.0;
  for (int i = 1; i <= model.count_of_vertices; i++) {
    double distance = sqrt(pow(model.matrix_3d.matrix[i][0], 2) +
                           pow(model.matrix_3d.matrix[i][1], 2) +
                           pow(model.matrix_3d.matrix[i][2], 2));
    if (distance > maxDistance) {
      maxDistance = distance;
    }
  }
  if (scaleFactor == 0) scaleFactor = 0.001;
  scale = (1.0 * scaleFactor) / maxDistance;
  for (int i = 1; i <= model.count_of_vertices; i++) {
    model.matrix_3d.matrix[i][0] *= scale;
    model.matrix_3d.matrix[i][1] *= scale;
    model.matrix_3d.matrix[i][2] *= scale;
  }
  doneCurrent();
  update();
}

void OpenGLWidget::editIntervalLines(double interval_value) {
  makeCurrent();
  glLineStipple(interval_value, 0x0F0F);
  doneCurrent();
  update();
}

void OpenGLWidget::editThicknessLines(double thickness_value) {
  makeCurrent();
  glLineWidth(thickness_value);
  doneCurrent();
  update();
}

void OpenGLWidget::setLineStyle(bool line) {
  makeCurrent();
  if (line == true) {
    useDottedLine = true;
  } else if (line == false) {
    useDottedLine = false;
  }
  doneCurrent();
  update();
}

void OpenGLWidget::verStyle(int dottedVer) {
  makeCurrent();
  if (dottedVer == 1) {
    useDottedVer = 1;
  } else if (dottedVer == 2) {
    useDottedVer = 2;
  } else if (dottedVer == 0) {
    useDottedVer = 0;
  }
  doneCurrent();
  update();
}

void OpenGLWidget::editSizeVer(double size_ver) {
  makeCurrent();
  glPointSize(size_ver);
  doneCurrent();
  update();
}

void OpenGLWidget::setBackgroundColor(const QColor &color) {
  makeCurrent();
  glClearColor(color.redF(), color.greenF(), color.blueF(), color.alphaF());
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  doneCurrent();
  update();
}

void OpenGLWidget::setColorLineVer(const QColor &color, bool type) {
  if (type == true) {
    lineColor = color;
  } else {
    pointColor = color;
  }
  update();
}

void OpenGLWidget::rotateModel(double step, char xyz) {
  makeCurrent();
  switch (xyz) {
    case 'x':
      RotateModel(&model, step, 'x');
      break;
    case 'y':
      RotateModel(&model, step, 'y');
      break;
    case 'z':
      RotateModel(&model, step, 'z');
      break;
    case 'X':
      RotateModel(&model, step, 'X');
      break;
    case 'Y':
      RotateModel(&model, step, 'Y');
      break;
    case 'Z':
      RotateModel(&model, step, 'Z');
      break;
    default:
      break;
  }
  ApplyRotation(&model);
  doneCurrent();
  update();
}

void OpenGLWidget::moveModel(double step, char xyz) {
  makeCurrent();
  switch (xyz) {
    case 'x':
      MoveModel(&model, step, 'x');
      break;
    case 'y':
      MoveModel(&model, step, 'y');
      break;
    case 'z':
      MoveModel(&model, step, 'z');
      break;
    case 'X':
      MoveModel(&model, step, 'X');
      break;
    case 'Y':
      MoveModel(&model, step, 'Y');
      break;
    case 'Z':
      MoveModel(&model, step, 'Z');
      break;
    default:
      break;
  }
  doneCurrent();
  update();
}

void OpenGLWidget::saveImage(const QString &filename) {
  QImage image = grabFramebuffer();
  image.save(filename);
}

void OpenGLWidget::mousePressEvent(QMouseEvent *event) {
  lastMousePos = event->pos();
}

void OpenGLWidget::mouseMoveEvent(QMouseEvent *event) {
  int dx = event->x() - lastMousePos.x();
  int dy = event->y() - lastMousePos.y();
  rotateModel(dy * rotationSpeed, 'x');
  rotateModel(dx * rotationSpeed, 'y');
  lastMousePos = event->pos();
  update();
}

void OpenGLWidget::clearContent() { FreeMemory(&model); }
