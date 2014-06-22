#ifndef INCLUDE_H
#define INCLUDE_H

#include <cmath>
#include <cstdlib>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#define GLM_SWIZZLE
#define GLM_FORCE_RADIANS
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
using glm::dot;
using glm::highp;
using glm::lookAt;
using glm::mat4;
using glm::normalize;
using glm::perspective;
using glm::precision;
using glm::rotate;
using glm::row;
using glm::translate;
using glm::scale;
using glm::mat4_cast;
using glm::value_ptr;
using glm::dvec3;
using glm::vec3;
using glm::vec4;
using std::cerr;
using std::cout;
using std::endl;

#endif
