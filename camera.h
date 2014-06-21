#ifndef CAMERA_H
#define CAMERA_H

#include "include.h"

const float ROT_SCALE = 0.2f;
const float ROLL_AMOUNT = 2.f;
const float MOVE_AMOUNT = .1f;

template <typename T>
class Camera
{
public:
  tvec3<T,highp> eye;
  Camera()
    : eye(vec3(2,2,2))
  {}
  virtual tmat4x4<T,highp> getView() const
  {
    return translate(getMat(), eye);
  }
  virtual void mouseLook(T,T) = 0;
  virtual void doRoll(T) = 0;
  void move(tvec3<T,highp> t)
  {
    tvec3<T,highp> up, fwd, r;
    cameraAxes(up, fwd, r);
    eye += r * t.x + up * t.y + fwd * t.z;
  }
  void cameraAxes(tvec3<T,highp>& up_out, tvec3<T,highp>& fwd_out,
    tvec3<T,highp>& r_out) const
  {
    tmat4x4<T,highp> rot = getMat();
    up_out = normalize((tvec3<T,highp>)(rot * tvec4<T,highp>(0,MOVE_AMOUNT,0,0)).xyz());
    fwd_out = normalize((tvec3<T,highp>)(rot * tvec4<T,highp>(0,0,MOVE_AMOUNT,0)).xyz());
    r_out = normalize((tvec3<T,highp>)(rot * tvec4<T,highp>(MOVE_AMOUNT,0,0,0)).xyz());
  }
  virtual tmat4x4<T,highp> getMat() const = 0;
  void doKeys(GLFWwindow* window)
  {
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
      move(tvec3<T,highp>(0,0,1));
    }
    else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
      move(tvec3<T,highp>(0,0,-1));
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
      move(tvec3<T,highp>(1,0,0));
    }
    else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
      move(tvec3<T,highp>(-1,0,0));
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
      move(tvec3<T,highp>(0,1,0));
    }
    else if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
    {
      move(tvec3<T,highp>(0,-1,0));
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
  tmat4x4<T,highp> rot;
  EulerCamera() : rot(tmat4x4<T,highp>(1)) {}
  virtual void mouseLook(T dx, T dy)
  {
    rot = rotate(rotate(rot, dy * ROT_SCALE, tvec3<T,highp>(1,0,0)),
      -dx * ROT_SCALE, tvec3<T,highp>(0,1,0));
  }
  virtual void doRoll(T dz)
  {
    rot = rotate(rot, dz * ROLL_AMOUNT, tvec3<T,highp>(0,0,1));
  }
  virtual tmat4x4<T,highp> getMat() const
  {
    return rot;
  }
};

template <typename T>
class QuatCamera : public Camera<T>
{
public:
  tquat<T,highp> quat;
  virtual void mouseLook(T dx,T dy)
  {
    quat = quat * (angleAxis(-dx * ROT_SCALE, tvec3<T,highp>(0,1,0)) *
      angleAxis(dy * ROT_SCALE, tvec3<T,highp>(1,0,0)));
    quat = normalize(quat);
  }
  virtual void doRoll(T dz)
  {
    quat = quat * angleAxis(dz * ROLL_AMOUNT, tvec3<T,highp>(0,0,1));
    quat = normalize(quat);
  }
  virtual tmat4x4<T,highp> getMat() const
  {
    return mat4_cast(quat);
  }
};

#endif
