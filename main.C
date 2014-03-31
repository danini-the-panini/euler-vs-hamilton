#include <cmath>
#include <cstdlib>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <iostream>
using glm::detail::tmat4x4;
using glm::detail::tquat;
using glm::detail::tvec3;
using glm::lookAt;
using glm::mat4;
using glm::perspective;
using glm::rotate;
using glm::scale;
using glm::value_ptr;
using glm::vec3;
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
template< typename T >
struct Camera
{
  tvec3<T> eye;
  tvec3<T> at;
  tvec3<T> up;
};
template< typename T >
tmat4x4<T> euler_rotate(T angle, tvec3<T> axis)
{
  return rotate(tmat4x4<T>(1.0), angle, normalize(axis));
}
template< typename T >
tmat4x4<T> quaternion_rotate(T angle, tvec3<T> axis)
{
  tquat<T> q = rotate(tquat<T>(1,0,0,0), angle, normalize(axis));
  return mat4_cast(q);
}
int main()
{
  const float fovy = 45.0f;
const float near = 1.f;
const float far = 100.0f;
const GLuint VERTEX_SIZE = 3;
const GLuint NUM_VERTICES = 4;
const GLuint VERTEX_ARRAY_SIZE = NUM_VERTICES * VERTEX_SIZE;
const GLuint NUM_FACES = 2;
const GLuint FACE_SIZE = 3;
const GLuint INDEX_ARRAY_SIZE = NUM_FACES * FACE_SIZE;
const GLuint POSITION_LOC = 1;
const char *vertex_glsl = 
  "#version 430\n"
  "uniform mat4 view;\n"
  "uniform mat4 projection;\n"
  "uniform mat4 world;\n"
  "layout (location=1) in vec3 position;\n"
  "void main()\n"
  "{\n"
    "gl_Position = projection * view * world * vec4(position,1.0f);\n"
  "}\n";
const char *fragment_glsl =
  "#version 430\n"
  "layout (location = 0) out vec4 colour;\n"
  "void main()\n"
  "{\n"
    "colour = vec4(1,0,1,1);\n"
  "}\n";
  GLFWwindow* window;
Camera<float> camera;
GLuint quad = 0;
float* verts = new float[VERTEX_ARRAY_SIZE];
float* faces = new float[INDEX_ARRAY_SIZE];
int width = 0;
int height = 0;
mat4 world(1);
GLuint shader_program = 0;
GLint view_location = 0;
GLint projection_location = 0;
GLint world_location = 0;
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
  camera.eye = vec3(2,2,2);
camera.at = vec3(0,0,0);
camera.up = vec3(0,1,0);
glClearColor(1,1,1,1);
int i = 0;
verts[i++] = 0.0f;
verts[i++] = 0.0f;
verts[i++] = 0.0f;
verts[i++] = 1.0f;
verts[i++] = 0.0f;
verts[i++] = 0.0f;
verts[i++] = 1.0f;
verts[i++] = 0.0f;
verts[i++] = 1.0f;
verts[i++] = 0.0f;
verts[i++] = 0.0f;
verts[i++] = 1.0f;
i = 0;
faces[i++] = 0;
faces[i++] = 2;
faces[i++] = 1;
faces[i++] = 0;
faces[i++] = 3;
faces[i++] = 2;
glGenVertexArrays(1, &quad);
glBindVertexArray(quad);
GLuint ptr;
glGenBuffers(1, &ptr);
glBindBuffer(GL_ARRAY_BUFFER, ptr);
glBufferData(GL_ARRAY_BUFFER, VERTEX_ARRAY_SIZE * sizeof(float),
        verts, GL_STATIC_DRAW);
glGenBuffers(1, &ptr);
glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ptr);
glBufferData(GL_ELEMENT_ARRAY_BUFFER, NUM_FACES * sizeof(GLuint),
        faces, GL_STATIC_DRAW);
glEnableVertexAttribArray(POSITION_LOC);
glVertexAttribPointer(POSITION_LOC, VERTEX_SIZE, GL_FLOAT, GL_FALSE,
        VERTEX_SIZE * sizeof(float), 0);
world = scale(world, vec3(10,10,10));
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
  /* Loop until the user closes the window */
  while (!glfwWindowShouldClose(window))
  {
      glfwGetFramebufferSize(window, &width, &height);
glViewport(0, 0, width, height);
glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
float ratio = (float) width / (float) height;
mat4 view = lookAt(camera.eye, camera.at, camera.up);
mat4 projection = perspective(fovy, ratio, near, far);
glUniformMatrix4fv(view_location, 1, GL_FALSE, value_ptr(view));
glUniformMatrix4fv(projection_location, 1, GL_FALSE, value_ptr(projection));
glUniformMatrix4fv(world_location, 1, GL_FALSE, value_ptr(world));
glBindVertexArray(quad);
glDrawElements(GL_TRIANGLES, (GLsizei)NUM_FACES, GL_UNSIGNED_INT, 0);
      /* Swap front and back buffers */
      glfwSwapBuffers(window);
      /* Poll for and process events */
      glfwPollEvents();
  }
  glfwTerminate();
  return 0;
}
