#include "vtk_output.h"

#ifdef ENABLE_VTK

#undef __DEPRECATED
#include <vtkSmartPointer.h>
#include <vtkRectilinearGrid.h>
#include <vtkDoubleArray.h>
#include <vtkIntArray.h>
#include <vtkCellData.h>
#include <vtkPointData.h>
#include <vtkXMLRectilinearGridWriter.h>
#define __DEPRECATED

// Writes 3D grid to vtk rectilinear grid output.
void write_as_vtk(std::string output, const Array3f &grid,
                  const Vec3f &min_box, const Vec3f &max_box) {
    auto rect_grid = vtkSmartPointer<vtkRectilinearGrid>::New();
    rect_grid->SetExtent(1, grid.ni, 1, grid.nj, 1, grid.nk);

    auto x_coord = vtkSmartPointer<vtkDoubleArray>::New();
    auto y_coord = vtkSmartPointer<vtkDoubleArray>::New();
    auto z_coord = vtkSmartPointer<vtkDoubleArray>::New();
    auto dx = (max_box[0]-min_box[0]) / (grid.ni-1);
    auto dy = (max_box[1]-min_box[1]) / (grid.nj-1);
    auto dz = (max_box[2]-min_box[2]) / (grid.nk-1);
    auto x=min_box[0], y=min_box[1], z=min_box[2];
    for (int i=0; i<grid.ni; ++i) {
        x_coord->InsertNextValue(x);
        x += dx;
    }
    for (int i=0; i<grid.nj; ++i) {
        y_coord->InsertNextValue(y);
        y += dy;
    }
    for (int i=0; i<grid.nk; ++i) {
        z_coord->InsertNextValue(z);
        z += dz;
    }
    rect_grid->SetXCoordinates(x_coord);
    rect_grid->SetYCoordinates(y_coord);
    rect_grid->SetZCoordinates(z_coord);


    auto phi = vtkSmartPointer<vtkDoubleArray>::New();
    phi->SetName("phi");
    phi->SetNumberOfComponents(1);
    phi->SetNumberOfTuples(grid.ni*grid.nj*grid.nk);

    for (int i=0; i<grid.ni*grid.nj*grid.nk; ++i) {
        phi->SetComponent(i, 0, grid.a[i]);
    }
    rect_grid->GetPointData()->AddArray(phi);

    auto writer = vtkSmartPointer<vtkXMLRectilinearGridWriter>::New();
    //! @todo user selects result file name.
    writer->SetFileName(output.c_str());
    writer->SetInput(rect_grid);
    writer->Write();
}
#else
void write_as_vtk(std::string output, const Array3f &grid,
                  const Vec3f &min_box, const Vec3f &max_box) {
    std::cerr << "Error: Not built with ENABLE_VTK\n";
}
#endif
