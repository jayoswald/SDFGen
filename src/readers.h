#pragma once
#include <vector>
#include "vec.h"

// Vertices and face connectivity for a triangulated mesh.
struct Triangulation {
    // List of vertices defining each triangle.
    std::vector<Vec3f>  vertList;
    // Face connectivity list for reach triangle.
    std::vector<Vec3ui> faceList;
    // Vectors defining the bounding box of the mesh (includes padding).
    Vec3f min_box, max_box;
};

// Reads input mesh data from a STL file (ascii format).
Triangulation read_ascii_stl(std::string filename);
// Reads input mesh data from a STL file (binary format).
Triangulation read_binary_stl(std::string filename);
// Reads input mesh data from a Wavefront OBJ (text) file.
Triangulation read_obj_file(std::string filename);

