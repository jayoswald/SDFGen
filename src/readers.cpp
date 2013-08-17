#include "readers.h"
#include "string_tools.h"

using std::cout;




// Reads the next vertex line in the STL file.
Vec3f read_stl_vertex(std::fstream &infile) {
    while (infile) {
        auto line = split(lower(read_line(infile)));
        if (line.empty()) continue;
        // vertex vx vy vz
        else if (line[0] == "vertex" && line.size() == 4) {
            return Vec3f{from_string<float>(line[1]),
                         from_string<float>(line[2]),
                         from_string<float>(line[3])};
        }
        else break;
    }
    std::cerr << "Unexpected ascii STL file format. Terminating.\n";
    exit(1);
    // Will never reach this point, but keep the compiler happy.
    return Vec3f{0,0,0};
}

// Reads input mesh data from a STL file (ascii format).
Triangulation read_ascii_stl(std::string filename) {
    Triangulation mesh;
    std::fstream infile(filename);

    // Skip header line.
    read_line(infile);
    while (infile) {
        // Read until a facet line is found
        while (infile) {
            auto line = split(lower(read_line(infile)));
            if (line.empty()) continue;
            // facet normal n_i n_j n_k
            else if (line.size() == 5 && line[0]=="facet") {
                auto n = Vec3f{from_string<float>(line[2]),
                               from_string<float>(line[3]),
                               from_string<float>(line[4])};
                auto v_ct = mesh.vertList.size();
                // Skip "outer loop"
                read_line(infile);
                for (int i=0; i<3; ++i) {
                    mesh.vertList.push_back(read_stl_vertex(infile));
                }
                mesh.faceList.emplace_back(v_ct, v_ct+1, v_ct+2);
                // Skip "endloop line".
                read_line(infile);
            }
            else if (line[0] == "endsolid") break;
        }
    }

    if (mesh.vertList.size()) {
        mesh.max_box = mesh.min_box =  mesh.vertList[0];
    }
    for (auto &v: mesh.vertList) {
        update_minmax(v, mesh.min_box, mesh.max_box);
    }
    cout << "Read in " << mesh.vertList.size() << " vertices and " 
         << mesh.faceList.size() << " faces.\n";
    return mesh;
}

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
            if (mesh.vertList.empty()) {
                mesh.max_box = mesh.min_box = point;
            }
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
    if (ignored_lines) {
        cout << "Warning: " << ignored_lines << " lines were ignored"
             << " since they did not contain faces or vertices.\n";
    }
    cout << "Read in " << mesh.vertList.size() << " vertices and " 
         << mesh.faceList.size() << " faces.\n";
    return mesh;
}

