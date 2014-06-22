#ifndef ORTHONORMALISE_H
#define ORTHONORMALISE_H

#include "include.h"

template<typename mat4_type>
mat4_type orthonormalise(mat4_type m)
{
  auto x = row(m,0).xyz();
  auto y = row(m,1).xyz();
  auto z = row(m,2).xyz();

  z = normalize(-cross(x,y));
  y = normalize(cross(x,z));

  return mat4_type(
      x.x, y.x, z.x, 0,
      x.y, y.y, z.y, 0,
      x.z, y.z, z.z, 0,
      0, 0, 0, 1
  );
}

#endif
