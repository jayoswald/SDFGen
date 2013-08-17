#pragma once
#include "vec.h"
#include "array3.h"
void write_as_vtk(std::string output, const Array3f &grid,
                  const Vec3f &min_box, const Vec3f &max_box);

