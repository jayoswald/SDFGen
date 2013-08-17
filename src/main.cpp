//SDFGen - A simple grid-based signed distance field generator for triangle meshes.
//Written by Christopher Batty (christopherbatty@yahoo.com, www.cs.columbia.edu/~batty)
//...primarily using code from Robert Bridson's website (www.cs.ubc.ca/~rbridson)
//This code is public domain. Feel free to mess with it, let me know if you like it.

#include "makelevelset3.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <limits>

using std::cout;

const char * help_msg = 
    "SDFGen - A utility for converting closed oriented triangle meshes\n"
    "         into grid-based signed distance fields.\n\n"
    "The output file format is:"
    "<ni> <nj> <nk>\n"
    "<origin_x> <origin_y> <origin_z>\n"
    "<dx>\n"
    "<value_1> <value_2> <value_3> [...]\n\n"
    
    "Where:\n"
    "  (ni,nj,nk) are the integer dimensions of the resulting distance field.\n"
    "  (origin_x,origin_y,origin_z) is the 3D position of the grid origin.\n"
    "  <dx> is the grid spacing.\n\n"
    "  <value_n> are the signed distance data values, in ascending order of i, then j, then k.\n"

    "The output filename will match that of the input, with the OBJ suffix replaced with SDF.\n\n"

    "Usage: SDFGen <filename> <dx> <padding>\n\n"
    "Where:\n"
    "  <filename> specifies a Wavefront OBJ (text) file representing a *triangle* mesh\n"
    "             (no quad or poly meshes allowed). File must use the suffix \".obj\".\n"
    "  <dx> specifies the length of grid cell in the resulting distance field.\n"
    "  <padding> specifies the number of cells worth of padding between the\n"
    "            object bound box and the boundary of the distance field grid.\n"
    "            Minimum is 1.\n\n";


// Vertices and face connectivity for a triangulated mesh.
struct Triangulation {
    std::vector<Vec3f> vertList;
    std::vector<Vec3ui> faceList;
    Vec3f min_box, max_box;
};

// Reads input mesh data from a Wavefront OBJ (text) file.
Triangulation read_obj_file(std::string filename) {
    Triangulation mesh;
    cout << "Reading data from " << filename << "\n";
    std::ifstream infile(filename);
    if(!infile) {
        std::cerr << "Failed to open " << filename << ". Terminating.\n";
        exit(-1);
    }

    int ignored_lines = 0;
    while(!infile.eof()) {
        std::string line;
        std::getline(infile, line);
        if(line.substr(0,1) == std::string("v")) {
            std::stringstream data(line);
            char c;
            Vec3f point;
            data >> c >> point[0] >> point[1] >> point[2];
            mesh.vertList.push_back(point);
            update_minmax(point, mesh.min_box, mesh.max_box);
        }
        else if(line.substr(0,1) == std::string("f")) {
            std::stringstream data(line);
            char c;
            int v0,v1,v2;
            data >> c >> v0 >> v1 >> v2;
            mesh.faceList.push_back(Vec3ui(v0-1,v1-1,v2-1));
        }
        else ++ignored_lines; 
    }
    infile.close();
    if (ignored_lines) {
        cout << "Warning: " << ignored_lines << " lines were ignored"
             << " since they did not contain faces or vertices.\n";
    }

    cout << "Read in " << mesh.vertList.size() << " vertices and " 
         << mesh.faceList.size() << " faces.\n";
}


int main(int argc, char** argv) {
  
  if (argc != 4) {
    cout << help_msg;
    exit(-1);
  }

  const std::string filename(argv[1]);
  if (filename.size() < 5 || filename.substr(filename.size()-4) != std::string(".obj")) {
    std::cerr << "Error: Expected OBJ file with filename of the form <name>.obj.\n";
    exit(-1);
  }

  std::stringstream arg2(argv[2]);
  float dx;
  arg2 >> dx;
  
  std::stringstream arg3(argv[3]);
  int padding;
  arg3 >> padding;


  Triangulation mesh = read_obj_file(filename);
  
  // Add padding around the box.
  Vec3f unit(1.0,1.0,1.0);
  if (padding < 1) padding = 1;
  mesh.min_box -= padding*dx*unit;
  mesh.max_box += padding*dx*unit;
  Vec3ui sizes = Vec3ui((mesh.max_box - mesh.min_box)/dx);
  
  cout << "Bound box size: (" << mesh.min_box << ") to (" 
       << mesh.max_box << ") with dimensions " << sizes << ".\n";

  cout << "Computing signed distance field.\n";
  Array3f phi_grid;
  make_level_set3(mesh.faceList, mesh.vertList, mesh.min_box, 
                  dx, sizes[0], sizes[1], sizes[2], phi_grid);

  // Very hackily strip off file suffix.
  std::string outname = filename.substr(0, filename.size()-4) + std::string(".sdf");
  cout << "Writing results to: " << outname << "\n";
  std::ofstream outfile(outname);
  outfile << phi_grid.ni << " " << phi_grid.nj << " " << phi_grid.nk << "\n";
  outfile << mesh.min_box[0] << " " 
          << mesh.min_box[1] << " " 
          << mesh.min_box[2] << "\n";
  outfile << dx << "\n";
  for (auto a: phi_grid.a) outfile << a << "\n";
  cout << "Processing complete.\n";
}

