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

// Just some helper functions for checking shaders...
void shaderLog(GLuint handle)
{
  int status;
  glGetShaderiv(handle, GL_COMPILE_STATUS, &status);
  if (status == GL_FALSE)
  {
     /* Get the length of the info log. */
     int len;
     glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &len);
     /* Get the info log. */
     char* log = new char[len];
     glGetShaderInfoLog(handle, len, &len, log);
     /* Print error */
     cerr << "Compilation error: " << log << endl;
     /* Finally, free the memory allocated. */
     delete log;
     /* Exit the program. */
     exit(-1);
  }
}
void programLog(GLuint handle)
{
  int status;
  glGetProgramiv(handle, GL_LINK_STATUS, &status);
  if (status == GL_FALSE)
  {
     /* Get the length of the info log. */
     int len;
     glGetProgramiv(handle, GL_INFO_LOG_LENGTH, &len);
     /* Get the info log. */
     char* log = new char[len];
     glGetProgramInfoLog(handle, len, &len, log);
     /* Print error */
     cerr << "Link error: " << log << endl;
     /* Finally, free the memory allocated. */
     delete log;
     /* Exit the program. */
     exit(-1);
  }
}

const float fovy = 45.f;
const float near = 1.f;
const float far = -100.f;

const GLuint VERTEX_SIZE = 3;
const GLuint NUM_VERTICES = 4;
const GLuint VERTEX_ARRAY_SIZE = NUM_VERTICES * VERTEX_SIZE;
const GLuint NUM_FACES = 2;
const GLuint FACE_SIZE = 3;
const GLuint INDEX_ARRAY_SIZE = NUM_FACES * FACE_SIZE;
const GLuint POSITION_LOC = 1;

const char *vertex_glsl = 
  "#version 430\n"
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
  "#version 430\n"
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

const float ROT_SCALE = 0.1f;
const float ROLL_AMOUNT = 1.f;

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


template <typename T>
class Camera
{
public:
  tvec3<T> eye;
  Camera()
    : eye(vec3(2,2,2))
  {}
  virtual mat4 getView() const
  {
    tvec3<T> up, fwd, r;
    cameraAxes(up, fwd, r);
    return lookAt((vec3)eye, (vec3)(eye+fwd), (vec3)up);
  }
  virtual void mouseLook(T,T) = 0;
  virtual void doRoll(T) = 0;
  void move(tvec3<T> t)
  {
    tvec3<T> up, fwd, r;
    cameraAxes(up, fwd, r);
    eye += r * t.x + up * t.y + fwd * t.z;
  }
  void cameraAxes(tvec3<T>& up_out, tvec3<T>& fwd_out,
    tvec3<T>& r_out) const
  {
    tmat4x4<T> rot = getMat();
    up_out = normalize((tvec3<T>)(rot * tvec4<T>(0,1,0,0)).xyz());
    fwd_out = normalize((tvec3<T>)(rot * tvec4<T>(0,0,1,0)).xyz());
    r_out = normalize((tvec3<T>)(rot * tvec4<T>(1,0,0,0)).xyz());
  }
  virtual tmat4x4<T> getMat() const = 0;
  void doKeys(GLFWwindow* window)
  {
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
      move(tvec3<T>(0,0,1));
    }
    else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
      move(tvec3<T>(0,0,-1));
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
      move(tvec3<T>(1,0,0));
    }
    else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
      move(tvec3<T>(-1,0,0));
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
      move(tvec3<T>(0,1,0));
    }
    else if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
    {
      move(tvec3<T>(0,-1,0));
    }
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    {
      doRoll(-1);
    }
    else if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
    {
      doRoll(1);
    }
  }
};

template <typename T>
class EulerCamera : public Camera<T>
{
public:
  tmat4x4<T> rot;
  EulerCamera() : rot(tmat4x4<T>(1)) {}
  virtual void mouseLook(T dx, T dy)
  {
    rot = rotate(rotate(rot, dy, tvec3<T>(1,0,0)), -dx, tvec3<T>(0,1,0));
  }
  virtual void doRoll(T dz)
  {
    rot = rotate(rot, dz, tvec3<T>(0,0,1));
  }
  virtual tmat4x4<T> getMat() const
  {
    return rot;
  }
};

template <typename T>
class QuatCamera : public Camera<T>
{
public:
  tquat<T> quat;
  virtual void mouseLook(T dx,T dy)
  {
    quat = quat * (angleAxis(-dx * ROT_SCALE, tvec3<T>(0,1,0)) *
      angleAxis(dy * ROT_SCALE, tvec3<T>(1,0,0)));
  }
  virtual void doRoll(T dz)
  {
    quat = quat * angleAxis(dz * ROLL_AMOUNT, tvec3<T>(0,0,1));
  }
  virtual tmat4x4<T> getMat() const
  {
    return mat4_cast(quat);
  }
};

Camera<float>
  *ecamf = new EulerCamera<float>(),
  *qcamf = new QuatCamera<float>();
Camera<double>
  *ecamd = new EulerCamera<double>(),
  *qcamd = new QuatCamera<double>();

double mx = -1, my = -1;

void mouseMoved(GLFWwindow* window, double x, double y)
{
  if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS)
  {
    if (mx != -1 && my != -1)
    {
      double dx = x - mx;
      double dy = y - my;

      ecamf->mouseLook(dx,dy);
      qcamf->mouseLook(dx,dy);

      ecamd->mouseLook(dx,dy);
      qcamd->mouseLook(dx,dy);
    }

    mx = x; my = y;
  }
  else mx = my = -1;
}

template <typename T>
void drawQuarter(int top, int left, int width, int height, Camera<T>* cam)
{
  glViewport(top, left, width, height);
  
  float ratio = (float) width / (float) height;
  mat4 view = cam->getView();
  mat4 projection = perspective(fovy, ratio, near, far);

  glUniformMatrix4fv(view_location, 1, GL_FALSE, value_ptr(view));
  glUniformMatrix4fv(projection_location, 1, GL_FALSE, value_ptr(projection));

  glDrawElements(GL_TRIANGLES, (GLsizei)INDEX_ARRAY_SIZE, GL_UNSIGNED_INT, 0);

  cam->doKeys(window);
}

double getDifference(Camera<float>* camf, Camera<double>* camd)
{
  tmat4x4<double> matf = (tmat4x4<double>) camf->getMat();
  tmat4x4<double> matd = camd->getMat();

  double diff = 0;

  for (int i = 0; i < 4; i++)
  {
    tvec4<double> v = column(matf, i) - column(matd, i);

    diff += (v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
  }

  return diff/16.0;
}

int main(/*int argc, char ** argv*/)
{
  /* Initialize the library */
  if (!glfwInit())
      return -1;

  /* Create a windowed mode window and its OpenGL context */
  window = glfwCreateWindow(640, 480, "Euler vs Hamilton", NULL, NULL);
  if (!window)
  {
      glfwTerminate();
      return -1;
  }

  /* Make the window's context current */
  glfwMakeContextCurrent(window);

  glewExperimental = GL_TRUE; 
  glewInit();

  glfwSetCursorPosCallback(window, mouseMoved);

  glEnable(GL_VERTEX_ARRAY);
  glEnable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);

  glClearColor(1,1,1,1);

  // load shaders
  shader_program = glCreateProgram();

  GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_shader, 1, &vertex_glsl, NULL);
  glCompileShader(vertex_shader);
  shaderLog(vertex_shader);

  GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader, 1, &fragment_glsl, NULL);
  glCompileShader(fragment_shader);
  shaderLog(fragment_shader);

  glAttachShader(shader_program, vertex_shader);
  glAttachShader(shader_program, fragment_shader);

  glLinkProgram(shader_program);
  programLog(shader_program);

  glUseProgram(shader_program);

  view_location = glGetUniformLocation(shader_program, "view");
  projection_location = glGetUniformLocation(shader_program, "projection");
  world_location = glGetUniformLocation(shader_program, "world");

  // load geometry
  int i = 0;
  verts[i++] = -1.0f;
  verts[i++] = 0.0f;
  verts[i++] = -1.0f;

  verts[i++] = 1.0f;
  verts[i++] = 0.0f;
  verts[i++] = -1.0f;

  verts[i++] = 1.0f;
  verts[i++] = 0.0f;
  verts[i++] = 1.0f;

  verts[i++] = -1.0f;
  verts[i++] = 0.0f;
  verts[i++] = 1.0f;

  i = 0;
  faces[i++] = 0;
  faces[i++] = 2;
  faces[i++] = 1;

  faces[i++] = 0;
  faces[i++] = 3;
  faces[i++] = 2;

  GLuint vptr, iptr;

  glGenVertexArrays(1, &quad);
  glBindVertexArray(quad);

  glGenBuffers(1, &vptr);
  glBindBuffer(GL_ARRAY_BUFFER, vptr);

  glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)(VERTEX_ARRAY_SIZE *
    sizeof(GLfloat)), verts, GL_STATIC_DRAW);

  glGenBuffers(1, &iptr);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iptr);

  glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)(INDEX_ARRAY_SIZE *
    sizeof(GLuint)), faces, GL_STATIC_DRAW);

  glEnableVertexAttribArray(POSITION_LOC);
  glVertexAttribPointer(POSITION_LOC, VERTEX_SIZE, GL_FLOAT, GL_FALSE,
          (GLsizei)(VERTEX_SIZE * sizeof(GLfloat)), 0);

  world = scale(world, vec3(10,10,10));

  glUniformMatrix4fv(world_location, 1, GL_FALSE, value_ptr(world));

  cout << ("Euler, Quaternion") << endl;

  /* Loop until the user closes the window */
  while (!glfwWindowShouldClose(window))
  {
    glfwGetFramebufferSize(window, &width, &height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    drawQuarter(0, 0, width/2, height/2, ecamf);
    drawQuarter(width/2, 0, width/2, height/2, ecamd);
    drawQuarter(0, height/2, width/2, height/2, qcamf);
    drawQuarter(width/2, height/2, width/2, height/2, qcamd);

    double ediff = getDifference(ecamf, ecamd);
    double qdiff = getDifference(qcamf, qcamd);

    cout.precision(15);
    cout << std::fixed << ediff << ", " << qdiff << endl;

    /* Swap front and back buffers */
    glfwSwapBuffers(window);
    /* Poll for and process events */
    glfwPollEvents();
  }

  glfwTerminate();

  return 0;
}
