#include "shader.h"

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


