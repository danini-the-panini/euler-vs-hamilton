#ifndef SHADER_H
#define SHADER_H

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

#endif
