/*
# $Id: ItkVtk.cpp,v 1.3 2004/06/07 22:33:29 nacholarrabide Exp $
# SkullyDoo - Segmentador y visualizador de imagenes tridimensionales  
# (C) 2002 Sebasti n Fiorentini / Ignacio Larrabide
# Contact Info: sebasfiorent@yahoo.com.ar / nacholarrabide@yahoo.com
# Argentina
############################# GPL LICENSE ####################################
#   This program is free software; you can redistribute it and/or modify      
#   it under the terms of the GNU General Public License as published by      
#   the Free Software Foundation; either version 2 of the License, or         
#   (at your option) any later version.                                       
#                                                                             
#   This program is distributed in the hope that it will be useful,           
#   but WITHOUT ANY WARRANTY; without even the implied warranty of            
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             
#   GNU General Public License for more details.                              
#                                                                             
#    You should have received a copy of the GNU General Public License
#    along with this program; if not, write to the Free Software
#    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
##############################################################################
*/

#include "ItkVtk.h"
#include "gui/ProgressWindowGUI.h"
#include <itkTriangleCell.h>
#include <itkQuadrilateralCell.h>
#include <vtkDataSet.h>
#include <vtkCellArray.h>


template <typename itkExporter>
void connectItkToVtkPipeline(itkExporter exporter,vtkImageImport* importer){
	importer->SetUpdateInformationCallback(exporter->GetUpdateInformationCallback());
	importer->SetPipelineModifiedCallback(exporter->GetPipelineModifiedCallback());
	importer->SetWholeExtentCallback(exporter->GetWholeExtentCallback());
	importer->SetSpacingCallback(exporter->GetSpacingCallback());
	importer->SetOriginCallback(exporter->GetOriginCallback());
	importer->SetScalarTypeCallback(exporter->GetScalarTypeCallback());
	importer->SetNumberOfComponentsCallback(exporter->GetNumberOfComponentsCallback());
	importer->SetPropagateUpdateExtentCallback(exporter->GetPropagateUpdateExtentCallback());
	importer->SetUpdateDataCallback(exporter->GetUpdateDataCallback());
	importer->SetDataExtentCallback(exporter->GetDataExtentCallback());
	importer->SetBufferPointerCallback(exporter->GetBufferPointerCallback());
	importer->SetCallbackUserData(exporter->GetCallbackUserData());
}

ItkVtk::ItkVtk(){
	vtkImageImporter=vtkImageImport::New();
	vtkImageImporter->SetDataScalarType(VTK_VoxelType);
	ProgressWindowGUI::Instance()->Observe(vtkImageImporter,"Conectando pipelines ITK->VTK","unknown");
	itkImageExporter=itkVtkVolumeExporterType::New();
	connectItkToVtkPipeline(itkImageExporter,vtkImageImporter);
}

ItkVtk::~ItkVtk(){
	vtkImageImporter->Delete();
}

void ItkVtk::setup(ItkVtk::itkImageType* input,vtkImageData* output){
	vtkImageImporter->SetOutput(output);
	itkImageExporter->SetInput(input);
	const itk::ImageBase<3>::SpacingType spacing=input->GetSpacing();
	const itk::ImageBase<3>::PointType origin=input->GetOrigin();
	output->SetSpacing(spacing[0],spacing[1],spacing[2]);
	output->SetOrigin(origin[0],origin[1],origin[2]);
}

//Convierte malla itk en malla vtk
class VistVTKCellsClass{
  vtkCellArray* m_Cells;
  int* m_LastCell;
  int* m_TypeArray;
public:
  // typedef the itk cells we are interested in
  typedef itk::CellInterface<ItkVtk::itkMeshType::PixelType,ItkVtk::itkMeshType::CellTraits >  CellInterfaceType;
  typedef itk::TriangleCell<CellInterfaceType> floatTriangleCell;
  typedef itk::QuadrilateralCell<CellInterfaceType> floatQuadrilateralCell;

  // Set the vtkCellArray that will be constructed
  void SetCellArray(vtkCellArray* a){
      m_Cells = a;
    }
  // Set the cell counter pointer
  void SetCellCounter(int* i){
      m_LastCell = i;
    }
  // Set the type array for storing the vtk cell types
  void SetTypeArray(int* i)
    {
      m_TypeArray = i;
    }
  // Visit a triangle and create the VTK_TRIANGLE cell 
  void Visit(unsigned long cellId, floatTriangleCell* t)
    {
      m_Cells->InsertNextCell(3,  (vtkIdType*)t->PointIdsBegin());
      m_TypeArray[*m_LastCell] = VTK_TRIANGLE;
      (*m_LastCell)++;
    }
  // Visit a triangle and create the VTK_QUAD cell 
  void Visit(unsigned long cellId, floatQuadrilateralCell* t)
    {
      m_Cells->InsertNextCell(4,  (vtkIdType*)t->PointIdsBegin());
      m_TypeArray[*m_LastCell] = VTK_QUAD;
      (*m_LastCell)++;
    }
};
  
typedef itk::CellInterfaceVisitorImplementation<
double, ItkVtk::itkMeshType::CellTraits,
itk::TriangleCell< itk::CellInterface<ItkVtk::itkMeshType::PixelType, ItkVtk::itkMeshType::CellTraits > >, 
  VistVTKCellsClass> TriangleVisitor;


typedef itk::CellInterfaceVisitorImplementation<
double, ItkVtk::itkMeshType::CellTraits,
  itk::QuadrilateralCell< itk::CellInterface<ItkVtk::itkMeshType::PixelType, ItkVtk::itkMeshType::CellTraits > >, 
  VistVTKCellsClass> QuadrilateralVisitor;

void ItkVtk::setup(itkMeshType* input,vtkPolyData* output){
  // Get the number of points in the mesh
  int numPoints = input->GetNumberOfPoints();
  if(numPoints == 0) return;
  // Create the vtkPoints object and set the number of points
  vtkPoints* vpoints = vtkPoints::New();
  vpoints->SetNumberOfPoints(numPoints);
  // iterate over all the points in the itk mesh filling in
  // the vtkPoints object as we go
  ItkVtk::itkMeshType::PointsContainer::Pointer points = input->GetPoints();
  for(ItkVtk::itkMeshType::PointsContainer::Iterator i = points->Begin();
      i != points->End(); ++i){
    // Get the point index from the point container iterator
    int idx = i->Index();
    // Set the vtk point at the index with the the coord array from itk
    // itk returns a const pointer, but vtk is not const correct, so
    // we have to use a const cast to get rid of the const
    vpoints->SetPoint(idx, const_cast<float*>(i->Value().GetDataPointer()));
    }
  // Set the points on the vtk grid
  output->SetPoints(vpoints);
  
  // Now create the cells using the MulitVisitor
  // 1. Create a MultiVisitor
  ItkVtk::itkMeshType::CellType::MultiVisitor::Pointer mv =ItkVtk::itkMeshType::CellType::MultiVisitor::New();
  // 2. Create a triangle and quadrilateral visitor
  TriangleVisitor::Pointer tv = TriangleVisitor::New();
  QuadrilateralVisitor::Pointer qv =  QuadrilateralVisitor::New();
  // 3. Set up the visitors
  int vtkCellCount = 0; // running counter for current cell being inserted into vtk
  int numCells = input->GetNumberOfCells();
  int *types = new int[numCells]; // type array for vtk 
  // create vtk cells and estimate the size
  vtkCellArray* cells = vtkCellArray::New();
  cells->EstimateSize(numCells, 4);
  // Set the TypeArray CellCount and CellArray for both visitors
  tv->SetTypeArray(types);
  tv->SetCellCounter(&vtkCellCount);
  tv->SetCellArray(cells);
  qv->SetTypeArray(types);
  qv->SetCellCounter(&vtkCellCount);
  qv->SetCellArray(cells);
  // add the visitors to the multivisitor
  mv->AddVisitor(tv);
  mv->AddVisitor(qv);
  // Now ask the mesh to accept the multivisitor which
  // will Call Visit for each cell in the mesh that matches the
  // cell types of the visitors added to the MultiVisitor
  input->Accept(mv);
  
  // Now set the cells on the vtk grid with the type array and cell array
  //output->SetCellData()
  output->SetPolys(cells);
  
  // Clean up vtk objects (no vtkSmartPointer ... )
  cells->Delete();
  vpoints->Delete();

}
