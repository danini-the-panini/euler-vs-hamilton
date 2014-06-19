#ifndef MAIN_H
#define MAIN_H

#include <cmath>
#include <cstdlib>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#define GLM_SWIZZLE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <iostream>

using glm::angleAxis;
using glm::column;
using glm::cross;
using glm::detail::tmat4x4;
using glm::detail::tquat;
using glm::detail::tvec3;
using glm::detail::tvec4;
using glm::highp;
using glm::lookAt;
using glm::mat4;
using glm::normalize;
using glm::perspective;
using glm::rotate;
using glm::scale;
using glm::mat4_cast;
using glm::value_ptr;
using glm::vec3;
using glm::vec4;
using std::cerr;
using std::cout;
using std::endl;

const char *vertex_glsl =
  "#version 330\n"
  "uniform mat4 view;"
  "uniform mat4 projection;"
  "uniform mat4 world;"
  "layout (location=1) in vec3 position;"
  "out vec3 world_position;"
  "void main()"
  "{"
    "world_position = (world * vec4(position,1.0f)).xyz;"
    "gl_Position = projection * view * world * vec4(position,1.0f);"
    // "gl_Position = vec4(position,1.0f);\n"
  "}";
const char *fragment_glsl =
  "#version 330\n"
  "layout (location = 0) out vec4 colour;"
  "in vec3 world_position;"
  "void main()"
  "{"
    "vec3 wp = world_position + vec3(100,100,100);"
    "float scale = 0.5;"
    "float xm = mod(int(wp.x*scale),2);"
    "float ym = mod(int(wp.y*scale),2);"
    "float zm = mod(int(wp.z*scale),2);"
    "float ip = 0;"
    "if ("
      "xm == zm"
      ") ip = 0.5;"
    "colour = vec4(ip,ip,ip,1);"
  "}";

void shaderLog(GLuint handle);
void programLog(GLuint handle);

const float fovy = 45.f;
const float near = 1.f;
const float far = -100.f;

const float ROT_SCALE = 0.2f;
const float ROLL_AMOUNT = 2.f;

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

#endif
