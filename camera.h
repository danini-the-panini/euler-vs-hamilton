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
  virtual mat4_type getView() const
  {
    vec3_type up, fwd, r;
    cameraAxes(up, fwd, r);

    return mat4_type (
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
 * Camera implementation using orthonormal axes representation.
 */
template <typename T, precision P = highp>
class AxisCamera : public Camera<T,P>
{
public:
  typedef tvec3<T,P> vec3_type;
  typedef tvec4<T,P> vec4_type;
  typedef tmat4x4<T,P> mat4_type;

  AxisCamera(vec3_type eye = vec3_type(0,0,0))
    : Camera<T,P>(eye), _s(1,0,0), _u(0,1,0), _f(0,0,1) {}
  virtual mat4_type getView()
  {
    return mat4_type (
      _s.x, _u.x, -_f.x, .0f,
      _s.y, _u.y, -_f.y, .0f,
      _s.z, _u.z, -_f.z, .0f,
      -dot(_s, this->_eye), -dot(_u, this->_eye), dot(_f, this->_eye), 1.f
    );
  }
  virtual void mouseLook(T dx, T dy)
  {
    _f = normalize((rotate(rotate(mat4_type(1), dx * ROT_SCALE, _u),
          dy * ROT_SCALE, _s) * vec4_type(_f, 0)).xyz());
    _s = normalize(cross(_u,_f));
    _u = normalize(_f,_s);
  }
  virtual void doRoll(T dz)
  {
    _u = normalize((rotate(mat4_type(1), dz * ROT_SCALE, _f) * vec4_type(_u, 0)).xyz());
    _s = normalize(cross(_u,_f));
  }
  virtual mat4_type getMat() const
  {
    return mat4_type (
      _s.x, _u.x, -_f.x, .0f,
      _s.y, _u.y, -_f.y, .0f,
      _s.z, _u.z, -_f.z, .0f,
      .0f, .0f, .0f, 1.f
    );
  }
private:
  vec3_type _u, _s, _f;
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
    _rot = rotate(rotate(_rot, dy * ROT_SCALE, vec3_type(1,0,0)),
        dx * ROT_SCALE, vec3_type(0,1,0));
  }
  virtual void doRoll(T dz)
  {
    _rot = rotate(_rot, -dz * ROLL_AMOUNT, vec3_type(0,0,1));
  }
  virtual mat4_type getMat() const
  {
    return _rot;
  }
private:
  mat4_type _rot;
};

/**
 * Camera implementation using the Rotation Matrix representation with orthonormalisation.
 */
template <typename T, precision P = highp>
class OrthoRotMatCamera : public Camera<T,P>
{
public:
  typedef tvec3<T,P> vec3_type;
  typedef tvec4<T,P> vec4_type;
  typedef tmat4x4<T,P> mat4_type;

  OrthoRotMatCamera(vec3_type eye = vec3_type(0,0,0))
    : Camera<T,P>(eye), _rot(mat4_type(1)) {}
  virtual void mouseLook(T dx, T dy)
  {
    _rot = rotate(rotate(_rot, dy * ROT_SCALE, vec3_type(1,0,0)),
        dx * ROT_SCALE, vec3_type(0,1,0));
    _rot = orthonormalise_gram_schmidt(_rot);
  }
  virtual void doRoll(T dz)
  {
    _rot = rotate(_rot, -dz * ROLL_AMOUNT, vec3_type(0,0,1));
    _rot = orthonormalise_gram_schmidt(_rot);
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
