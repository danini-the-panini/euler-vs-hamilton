#ifndef SHADER_H
#define SHADER_H

#include "include.h"

extern const char *vertex_glsl;
extern const char *fragment_glsl;

void shaderLog(GLuint handle);
void programLog(GLuint handle);


#endif
