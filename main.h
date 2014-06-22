#ifndef MAIN_H
#define MAIN_H

#include "include.h"
#include "camera.h"
#include "shader.h"

const float FOVY = 45.f;
const float NEAR = 1.f;
const float FAR = -100.f;

const GLuint VERTEX_SIZE = 3;
const GLuint NUM_VERTICES = 4;
const GLuint VERTEX_ARRAY_SIZE = NUM_VERTICES * VERTEX_SIZE;
const GLuint NUM_FACES = 2;
const GLuint FACE_SIZE = 3;
const GLuint INDEX_ARRAY_SIZE = NUM_FACES * FACE_SIZE;
const GLuint POSITION_LOC = 1;

GLFWwindow* window;

GLuint quad = 0;
GLfloat* verts = new GLfloat[VERTEX_ARRAY_SIZE];
GLuint* faces = new GLuint[INDEX_ARRAY_SIZE];

int width = 0;
int height = 0;

mat4 world(1);

GLuint shader_program = 0;

GLint view_location = 0;
GLint projection_location = 0;
GLint world_location = 0;

Camera<float>
  *ecamf = new EulerCamera<float>(),
  *qcamf = new QuatCamera<float>();
Camera<double>
  *ecamd = new EulerCamera<double>(),
  *qcamd = new QuatCamera<double>();

double mx = -1, my = -1;

void mouseMoved(GLFWwindow* window, double x, double y);

double getDifference(Camera<float>* camf, Camera<double>* camd);

template <typename T>
void drawQuarter(int top, int left, int w, int h, Camera<T>* cam)
{
  glViewport(top, left, w, h);

  float ratio = (float) w / (float) h;
  mat4 view(cam->getView());
  mat4 projection = perspective(FOVY, ratio, NEAR, FAR);

  glUniformMatrix4fv(view_location, 1, GL_FALSE, value_ptr(view));
  glUniformMatrix4fv(projection_location, 1, GL_FALSE, value_ptr(projection));

  glDrawElements(GL_TRIANGLES, (GLsizei)INDEX_ARRAY_SIZE, GL_UNSIGNED_INT, 0);

  cam->doKeys(window);
}

void init();
void initGlfw();
void initWindow();
void initGlew();

void loadShaders();
void loadGeometry();

void mainLoop();

#endif
