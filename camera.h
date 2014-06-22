#ifndef CAMERA_H
#define CAMERA_H

#include "include.h"
#include "orthonormalise.h"

const float ROT_SCALE = 0.00349f;
const float ROLL_AMOUNT = .0349f;
const float MOVE_AMOUNT = .5f;

template <typename T, precision P = highp>
class Camera
{
public:
  typedef tvec3<T,P> vec3_type;
  typedef tvec4<T,P> vec4_type;
  typedef tmat4x4<T,P> mat4_type;

  Camera(vec3_type eye = vec3_type(0,0,0))
    : _eye(eye)
  {}
  virtual tmat4x4<T,highp> getView() const
  {
    vec3_type up, fwd, r;
    cameraAxes(up, fwd, r);

    return tmat4x4<T,highp> (
      r.x, up.x, -fwd.x, .0f,
      r.y, up.y, -fwd.y, .0f,
      r.z, up.z, -fwd.z, .0f,
      -dot(r, _eye), -dot(up, _eye), dot(fwd, _eye), 1.f
    );
  }
  virtual void mouseLook(T,T) = 0;
  virtual void doRoll(T) = 0;
  void move(vec3_type t)
  {
    vec3_type up, fwd, r;
    cameraAxes(up, fwd, r);
    _eye += r * t.x + up * t.y + fwd * t.z;
  }
  void cameraAxes(vec3_type& up_out, vec3_type& fwd_out,
    vec3_type& r_out) const
  {
    mat4_type rot = getMat();
    up_out = normalize((vec3_type)(rot * vec4_type(0,1,0,0)).xyz());
    fwd_out = normalize((vec3_type)(rot * vec4_type(0,0,1,0)).xyz());
    r_out = normalize((vec3_type)(rot * vec4_type(1,0,0,0)).xyz());
  }
  virtual mat4_type getMat() const = 0;
  void doKey(int key)
  {
    switch(key)
    {
      case GLFW_KEY_W:
        move(vec3_type(0,0,MOVE_AMOUNT));
        break;
      case GLFW_KEY_S:
        move(vec3_type(0,0,-MOVE_AMOUNT));
        break;
      case GLFW_KEY_A:
        move(vec3_type(-MOVE_AMOUNT,0,0));
        break;
      case GLFW_KEY_D:
        move(vec3_type(MOVE_AMOUNT,0,0));
        break;
      case GLFW_KEY_SPACE:
        move(vec3_type(0,MOVE_AMOUNT,0));
        break;
      case GLFW_KEY_LEFT_CONTROL:
        move(vec3_type(0,-MOVE_AMOUNT,0));
        break;
      case GLFW_KEY_Q:
        doRoll(-1);
        break;
      case GLFW_KEY_E:
        doRoll(1);
        break;
    }
  }
protected:
  vec3_type _eye;
};

/**
 * Camera implementation using the Rotation Matrix representation.
 */
template <typename T, precision P = highp>
class RotMatCamera : public Camera<T,P>
{
public:
  typedef tvec3<T,P> vec3_type;
  typedef tvec4<T,P> vec4_type;
  typedef tmat4x4<T,P> mat4_type;

  RotMatCamera(vec3_type eye = vec3_type(0,0,0))
    : Camera<T,P>(eye), _rot(mat4_type(1)) {}
  virtual void mouseLook(T dx, T dy)
  {
    _rot = orthonormalise(rotate(rotate(_rot, dy * ROT_SCALE, vec3_type(1,0,0)),
      dx * ROT_SCALE, vec3_type(0,1,0)));
  }
  virtual void doRoll(T dz)
  {
    _rot = orthonormalise(rotate(_rot, -dz * ROLL_AMOUNT, vec3_type(0,0,1)));
  }
  virtual mat4_type getMat() const
  {
    return _rot;
  }
private:
  mat4_type _rot;
};

/**
 * Camera implementation using the Quaternion representation.
 */
template <typename T, precision P = highp>
class QuatCamera : public Camera<T,P>
{
public:
  typedef tvec3<T,P> vec3_type;
  typedef tvec4<T,P> vec4_type;
  typedef tmat4x4<T,P> mat4_type;

  QuatCamera(vec3_type eye = vec3_type(0,0,0)):
    Camera<T,P>(eye),
    _quat()
  {}

  virtual void mouseLook(T dx,T dy)
  {
    _quat = _quat * (angleAxis(dx * ROT_SCALE, vec3_type(0,1,0)) *
      angleAxis(dy * ROT_SCALE, vec3_type(1,0,0)));
    _quat = normalize(_quat);
  }
  virtual void doRoll(T dz)
  {
    _quat = _quat * angleAxis(-dz * ROLL_AMOUNT, vec3_type(0,0,1));
    _quat = normalize(_quat);
  }
  virtual mat4_type getMat() const
  {
    return mat4_cast(_quat);
  }
private:
  tquat<T,P> _quat;
};

#endif
