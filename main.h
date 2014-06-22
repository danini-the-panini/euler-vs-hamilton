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

typedef Camera<float,lowp> CamF;
typedef Camera<double,highp> CamD;

const CamF::vec3_type INITIAL_EYE_F(0,2,0);
const CamD::vec3_type INITIAL_EYE_D(0,2,0);

CamF
  *rcamf = new OrthoRotMatCamera<float,lowp>(INITIAL_EYE_F),
  *qcamf = new QuatCamera<float,lowp>(INITIAL_EYE_F);
CamD
  *rcamd = new OrthoRotMatCamera<double>(INITIAL_EYE_D),
  *qcamd = new QuatCamera<double>(INITIAL_EYE_D);

ifstream input_file;
ofstream output_file;

const int KEYS[] = {GLFW_KEY_W, GLFW_KEY_S, GLFW_KEY_A, GLFW_KEY_D,
  GLFW_KEY_SPACE, GLFW_KEY_LEFT_CONTROL, GLFW_KEY_E, GLFW_KEY_Q};

bool is_writing = false, is_reading = false;

bool mouse_ready = false;
double mx, my;

void handleArguments(int argc, char ** argv);

void mouseMoved(double x, double y);
void mouseCheck(GLFWwindow*,double,double);

double getDifference(CamF* camf, CamD* camd);

void handleInput();
void doKeyToCameras(int key);
void checkInputFile();

template <typename T, precision P = highp>
void drawQuarter(int top, int left, int w, int h, Camera<T,P>* cam)
{
  glViewport(top, left, w, h);

  float ratio = (float) w / (float) h;
  mat4 view(cam->getView());
  mat4 projection = perspective(FOVY, ratio, NEAR, FAR);

  glUniformMatrix4fv(view_location, 1, GL_FALSE, value_ptr(view));
  glUniformMatrix4fv(projection_location, 1, GL_FALSE, value_ptr(projection));

  glDrawElements(GL_TRIANGLES, (GLsizei)INDEX_ARRAY_SIZE, GL_UNSIGNED_INT, 0);
}

void init();
void initGlfw();
void initWindow();
void initGlew();

void loadShaders();
void loadGeometry();

void mainLoop();

#endif
