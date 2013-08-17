//SDFGen - A simple grid-based signed distance field generator for triangle meshes.
//Written by Christopher Batty (christopherbatty@yahoo.com, www.cs.columbia.edu/~batty)
//...primarily using code from Robert Bridson's website (www.cs.ubc.ca/~rbridson)
//This code is public domain. Feel free to mess with it, let me know if you like it.

#include "readers.h"
#include "string_tools.h"
#include "makelevelset3.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <limits>

using std::cout;

const char* help_msg = 
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



int main(int argc, char** argv) {
  
    if (argc != 4) {
        std::cerr << help_msg;
        exit(-1);
    }
    auto filename  = std::string{argv[1]};
    auto dx        = from_string<float>(argv[2]);
    auto padding   = from_string<int>(argv[3]);

    auto dot = filename.find_last_of('.');
    if (dot == std::string::npos) {
        std::cerr << "Error: Input file must have .stl or .obj extension.\n";
        exit(-1);
    }
    auto extension = filename.substr(dot+1);
    auto basename  = filename.substr(0, dot);
    auto outname   = basename + std::string(".sdf");
    
    cout << "File name is   " << filename << "\n";
    cout << "Extension is   " << extension << "\n";
    cout << "Base name is   " << basename << "\n";
    cout << "Output name is " << outname<< "\n";

    Triangulation mesh;
    if (lower(extension) == "stl") {
        mesh = read_ascii_stl(filename);
    }
    else if (lower(extension) == "obj") {
        mesh = read_obj_file(filename);
    }
    else {
        std::cerr << "Error: Input file must have .stl or .obj extension.\n";
        exit(-1);
    }

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

