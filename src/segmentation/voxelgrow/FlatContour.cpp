/*
# $Id: FlatContour.cpp,v 1.1 2003/05/02 22:21:59 sebasfiorent Exp $
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

#include "FlatContour.h"
#include <vtkMergePoints.h>
#include <vtkPointLocator.h>
#include <vtkDataArray.h>
#include "VoxelGrow.h"
#include <vtkFloatArray.h>
#include <vtkStructuredPoints.h>
#include <vtkPolygon.h>


FlatContour::FlatContour():vtkStructuredPointsToPolyDataFilter(){
	outsideValue=VoxelGrow::VALUE_REGION_OUT;
	centeredValues=true;
}

inline void FlatContour::loadarr(int pt[3],float x, float y, float z){
  pt[0]=(int)x;
  pt[1]=(int)y;
  pt[2]=(int)z;
}

inline void FlatContour::loadarr(float pt[3],float x, float y, float z){
  pt[0]=(int)x;
  pt[1]=(int)y;
  pt[2]=(int)z;
}

void FlatContour::setOutsideValue(float outsideValue)
{
	this->outsideValue=outsideValue;
}

void FlatContour::setCenteredValues()
{
	centeredValues=true;
}

void FlatContour::resetCenteredValues()
{
	centeredValues=false;
}


float FlatContour::getOutsideValue()
{
	return outsideValue;
}


void FlatContour::Execute(){
	vtkPolyData *output=GetOutput();
	vtkImageData *input = GetInput();
	int input_dim[3];
	float input_origin[3];
	float input_spacing[3];
	vtkPoints* puntos=vtkPoints::New();
	input->GetDimensions(input_dim);
	input->GetOrigin(input_origin);
	input->GetSpacing(input_spacing);
	int estimatedSize;
	estimatedSize = (int) pow ((double) (input_dim[0] * input_dim[1] * input_dim[2]), .75);
	estimatedSize = estimatedSize / 1024 * 1024; //multiple of 1024
	if (estimatedSize < 1024)  estimatedSize = 1024;
	puntos->Allocate(estimatedSize);
	
	const float offsX=0.5*input_spacing[0];
	const float offsY=0.5*input_spacing[1];
	const float offsZ=0.5*input_spacing[2];

	float bounds[6];
	input->GetBounds(bounds);
	bounds[0]=bounds[0]-offsX;
	bounds[1]=bounds[1]+offsX;
	bounds[2]=bounds[2]-offsY;
	bounds[3]=bounds[3]+offsY;
	bounds[4]=bounds[4]-offsZ;
	bounds[5]=bounds[5]+offsZ;
	
	vtkMergePoints *mp=vtkMergePoints::New();
	mp->InitPointInsertion(puntos,bounds,estimatedSize);
	vtkDataArray *scalars=input->GetPointData()->GetScalars();
	//vtkFloatArray* normales=vtkFloatArray::New();
	//normales->SetNumberOfComponents(3);
	vtkCellArray* cells=vtkCellArray::New();
	output->SetPolys(cells);
	output->SetPoints(puntos);
//	output->GetCellData()->SetNormals(normales);
	puntos->Delete();
	cells->Delete();
	//normales->Delete();

	
	int pos[3];
	int pointId;
	
	long totalpoints=input_dim[0]*input_dim[1]*input_dim[2];
	long actualpoint=0;
	long stepsize=totalpoints/20;

	//Comienzo la iteraci¾n. Recorro en x,y,z
	UpdateProgress(0);
	float marcaAct;
	float marcaVecX;
	float marcaVecY;
	float marcaVecZ;
	for (int x=0;x<input_dim[0];x++){
		for (int z=0;z<input_dim[2];z++){
			for (int y=0;y<input_dim[1];y++){
				
				actualpoint++;
				if (actualpoint%stepsize==0){
					UpdateProgress((float)actualpoint/(float)totalpoints);
				}
				
				pos[0]=x;
				pos[1]=y;
				pos[2]=z;
				
				pointId = input->ComputePointId(pos);
				marcaAct = scalars->GetComponent(pointId,0);
				int auxPos[3];
				marcaVecX = outsideValue;
				//El vecino esta dentro
				if (pos[0]+1<input_dim[0]){
					loadarr(auxPos,pos[0]+1,pos[1],pos[2]);
					pointId = input->ComputePointId(auxPos);
					marcaVecX = scalars->GetComponent(pointId,0);
				}
				marcaVecY = outsideValue;
				if (pos[1]+1<input_dim[1]){
					loadarr(auxPos,pos[0],pos[1]+1,pos[2]);
					pointId = input->ComputePointId(auxPos);
					marcaVecY = scalars->GetComponent(pointId,0);
				}
				marcaVecZ = outsideValue;
				if (pos[2]+1<input_dim[2]){
					loadarr(auxPos,pos[0],pos[1],pos[2]+1);
					pointId = input->ComputePointId(auxPos);
					marcaVecZ = scalars->GetComponent(pointId,0);
				}
				
				
				float vox_pos[3];
				vox_pos[0] = x*input_spacing[0];
				vox_pos[1] = y*input_spacing[1];
				vox_pos[2] = z*input_spacing[2];
				
				float pt1[3];
				float pt2[3];
				float pt3[3];  
				float pt4[3];
				
				int puntoscara[4];
				
				int tri1[3];
				int tri2[3];
				//Fijarse si hay que generar la cara anterior
				if (x==0 && marcaAct!=outsideValue){
					//creo los puntos de la cara anterior en X
					loadarr(pt1,vox_pos[0]-offsX,vox_pos[1]-offsY,vox_pos[2]-offsZ);
					loadarr(pt2,vox_pos[0]-offsX,vox_pos[1]-offsY,vox_pos[2]+offsZ);
					loadarr(pt3,vox_pos[0]-offsX,vox_pos[1]+offsY,vox_pos[2]+offsZ);
					loadarr(pt4,vox_pos[0]-offsX,vox_pos[1]+offsY,vox_pos[2]-offsZ);
					// agrego los puntos en el resultado
					mp->InsertUniquePoint(pt1,puntoscara[0]);
					mp->InsertUniquePoint(pt2,puntoscara[1]);
					mp->InsertUniquePoint(pt3,puntoscara[2]);
					mp->InsertUniquePoint(pt4,puntoscara[3]);
					//si estoy adentro
					tri1[0]=puntoscara[0];
					tri1[1]=puntoscara[1];
					tri1[2]=puntoscara[3];
					tri2[0]=puntoscara[1];
					tri2[1]=puntoscara[2];
					tri2[2]=puntoscara[3];
					float normal[3];
					int cid=output->InsertNextCell(VTK_TRIANGLE,3,tri1);
					vtkPolygon::ComputeNormal(puntos,3,tri1,normal);
					//normales->InsertTuple(cid,normal);
					cid=output->InsertNextCell(VTK_TRIANGLE,3,tri2);
					vtkPolygon::ComputeNormal(puntos,3,tri2,normal);
					//normales->InsertTuple(cid,normal);
				}
				//Fijarse si hay que generar la cara anterior
				if (y==0 && marcaAct!=outsideValue){
					//creo los puntos de la cara anterior en X
					//creo los puntos de la cara en Y
					loadarr(pt1,vox_pos[0]+offsX,vox_pos[1]-offsY,vox_pos[2]-offsZ);
					loadarr(pt2,vox_pos[0]+offsX,vox_pos[1]-offsY,vox_pos[2]+offsZ);
					loadarr(pt3,vox_pos[0]-offsX,vox_pos[1]-offsY,vox_pos[2]+offsZ);
					loadarr(pt4,vox_pos[0]-offsX,vox_pos[1]-offsY,vox_pos[2]-offsZ);
					// agrego los puntos en el resultado
					mp->InsertUniquePoint(pt1,puntoscara[0]);
					mp->InsertUniquePoint(pt2,puntoscara[1]);
					mp->InsertUniquePoint(pt3,puntoscara[2]);
					mp->InsertUniquePoint(pt4,puntoscara[3]);
					tri1[0]=puntoscara[0];
					tri1[1]=puntoscara[1];
					tri1[2]=puntoscara[2];
					
					tri2[0]=puntoscara[2];
					tri2[1]=puntoscara[3];
					tri2[2]=puntoscara[0];
					//si estoy adentro
					float normal[3];
					int cid=output->InsertNextCell(VTK_TRIANGLE,3,tri1);
					vtkPolygon::ComputeNormal(puntos,3,tri1,normal);
					//normales->InsertTuple(cid,normal);
					cid=output->InsertNextCell(VTK_TRIANGLE,3,tri2);
					vtkPolygon::ComputeNormal(puntos,3,tri2,normal);
					//normales->InsertTuple(cid,normal);
				}
				//Fijarse si hay que generar la cara anterior en Z
				if (z==0 && marcaAct!=outsideValue){
					//creo los puntos de la cara anterior en X
					//creo los puntos de la cara en Y
					loadarr(pt1,vox_pos[0]+offsX,vox_pos[1]-offsY,vox_pos[2]-offsZ);
					loadarr(pt2,vox_pos[0]+offsX,vox_pos[1]+offsY,vox_pos[2]-offsZ);
					loadarr(pt3,vox_pos[0]-offsX,vox_pos[1]+offsY,vox_pos[2]-offsZ);
					loadarr(pt4,vox_pos[0]-offsX,vox_pos[1]-offsY,vox_pos[2]-offsZ);
					
					// agrego los puntos en el resultado
					mp->InsertUniquePoint(pt1,puntoscara[0]);
					mp->InsertUniquePoint(pt2,puntoscara[1]);
					mp->InsertUniquePoint(pt3,puntoscara[2]);
					mp->InsertUniquePoint(pt4,puntoscara[3]);
					//si estoy adentro
					tri1[0]=puntoscara[0];
					tri1[1]=puntoscara[1];
					tri1[2]=puntoscara[2];
					tri2[0]=puntoscara[3];
					tri2[1]=puntoscara[3];
					tri2[2]=puntoscara[0];
					
					float normal[3];
					int cid=output->InsertNextCell(VTK_TRIANGLE,3,tri1);
					vtkPolygon::ComputeNormal(puntos,3,tri1,normal);
					//normales->InsertTuple(cid,normal);
					cid=output->InsertNextCell(VTK_TRIANGLE,3,tri2);
					vtkPolygon::ComputeNormal(puntos,3,tri2,normal);
					//normales->InsertTuple(cid,normal);
				}
				
				/**
				*/
				//si el vecino en x cambia y uno de los dos estß seleccionado (es = a 1), genero la cara
				if (marcaAct!=marcaVecX && (marcaAct==outsideValue || marcaVecX==outsideValue)){
					//creo los puntos de la cara en X
					loadarr(pt1,vox_pos[0]+offsX,vox_pos[1]-offsY,vox_pos[2]-offsZ);
					loadarr(pt2,vox_pos[0]+offsX,vox_pos[1]-offsY,vox_pos[2]+offsZ);
					loadarr(pt3,vox_pos[0]+offsX,vox_pos[1]+offsY,vox_pos[2]+offsZ);
					loadarr(pt4,vox_pos[0]+offsX,vox_pos[1]+offsY,vox_pos[2]-offsZ);
					
					// agrego los puntos en el resultado
					mp->InsertUniquePoint(pt1,puntoscara[0]);
					mp->InsertUniquePoint(pt2,puntoscara[1]);
					mp->InsertUniquePoint(pt3,puntoscara[2]);
					mp->InsertUniquePoint(pt4,puntoscara[3]);
					
					//si estoy afuera
					if (marcaAct!=outsideValue){
						tri1[0]=puntoscara[0];
						tri1[1]=puntoscara[3];
						tri1[2]=puntoscara[1];
						
						tri2[0]=puntoscara[1];
						tri2[1]=puntoscara[3];
						tri2[2]=puntoscara[2];
						float normal[3];
						int cid=output->InsertNextCell(VTK_TRIANGLE,3,tri1);
						vtkPolygon::ComputeNormal(puntos,3,tri1,normal);
						//normales->InsertTuple(cid,normal);
						cid=output->InsertNextCell(VTK_TRIANGLE,3,tri2);
						vtkPolygon::ComputeNormal(puntos,3,tri2,normal);
						//normales->InsertTuple(cid,normal);
					}else{
						tri1[0]=puntoscara[0];
						tri1[1]=puntoscara[1];
						tri1[2]=puntoscara[3];
						
						tri2[0]=puntoscara[1];
						tri2[1]=puntoscara[2];
						tri2[2]=puntoscara[3];
						float normal[3];
						int cid=output->InsertNextCell(VTK_TRIANGLE,3,tri1);
						vtkPolygon::ComputeNormal(puntos,3,tri1,normal);
						//normales->InsertTuple(cid,normal);
						cid=output->InsertNextCell(VTK_TRIANGLE,3,tri2);
						vtkPolygon::ComputeNormal(puntos,3,tri2,normal);
						//normales->InsertTuple(cid,normal);
					}
				}
				
				
				
				//si el vecino en Y cambia y uno de los dos estß seleccionado (es = a 1), genero la cara
				if (marcaAct!=marcaVecY && (marcaAct==outsideValue || marcaVecY==outsideValue)){
					//creo los puntos de la cara en Y
					loadarr(pt1,vox_pos[0]+offsX,vox_pos[1]+offsY,vox_pos[2]-offsZ);
					loadarr(pt2,vox_pos[0]+offsX,vox_pos[1]+offsY,vox_pos[2]+offsZ);
					loadarr(pt3,vox_pos[0]-offsX,vox_pos[1]+offsY,vox_pos[2]+offsZ);
					loadarr(pt4,vox_pos[0]-offsX,vox_pos[1]+offsY,vox_pos[2]-offsZ);
					
					// agrego los puntos en el resultado
					mp->InsertUniquePoint(pt1,puntoscara[0]);
					mp->InsertUniquePoint(pt2,puntoscara[1]);
					mp->InsertUniquePoint(pt3,puntoscara[2]);
					mp->InsertUniquePoint(pt4,puntoscara[3]);
					
					//si estoy afuera
					if (marcaAct!=outsideValue){
						tri1[0]=puntoscara[0];
						tri1[1]=puntoscara[1];
						tri1[2]=puntoscara[2];
						
						tri2[0]=puntoscara[2];
						tri2[1]=puntoscara[3];
						tri2[2]=puntoscara[0];
						float normal[3];
						int cid=output->InsertNextCell(VTK_TRIANGLE,3,tri1);
						vtkPolygon::ComputeNormal(puntos,3,tri1,normal);
						//normales->InsertTuple(cid,normal);
						cid=output->InsertNextCell(VTK_TRIANGLE,3,tri2);
						vtkPolygon::ComputeNormal(puntos,3,tri2,normal);
						//normales->InsertTuple(cid,normal);
					}else{
						tri1[0]=puntoscara[0];
						tri1[1]=puntoscara[2];
						tri1[2]=puntoscara[1];
						
						tri2[0]=puntoscara[3];
						tri2[1]=puntoscara[2];
						tri2[2]=puntoscara[0];
						float normal[3];
						int cid=output->InsertNextCell(VTK_TRIANGLE,3,tri1);
						vtkPolygon::ComputeNormal(puntos,3,tri1,normal);
						//normales->InsertTuple(cid,normal);
						cid=output->InsertNextCell(VTK_TRIANGLE,3,tri2);
						vtkPolygon::ComputeNormal(puntos,3,tri2,normal);
						//normales->InsertTuple(cid,normal);
					}
				}
				
				//si el vecino en Z cambia y uno de los dos estß seleccionado (es = a 1), genero la cara
				if (marcaAct!=marcaVecZ && (marcaAct==outsideValue || marcaVecZ==outsideValue)){
					//creo los puntos de la cara en Z
					loadarr(pt1,vox_pos[0]+offsX,vox_pos[1]-offsY,vox_pos[2]+offsZ);
					loadarr(pt2,vox_pos[0]+offsX,vox_pos[1]+offsY,vox_pos[2]+offsZ);
					loadarr(pt3,vox_pos[0]-offsX,vox_pos[1]+offsY,vox_pos[2]+offsZ);
					loadarr(pt4,vox_pos[0]-offsX,vox_pos[1]-offsY,vox_pos[2]+offsZ);
					
					// agrego los puntos en el resultado
					mp->InsertUniquePoint(pt1,puntoscara[0]);
					mp->InsertUniquePoint(pt2,puntoscara[1]);
					mp->InsertUniquePoint(pt3,puntoscara[2]);
					mp->InsertUniquePoint(pt4,puntoscara[3]);
					
					//si estoy afuera
					if (marcaAct!=outsideValue){
						tri1[0]=puntoscara[0];
						tri1[1]=puntoscara[1];
						tri1[2]=puntoscara[2];
						
						tri2[0]=puntoscara[2];
						tri2[1]=puntoscara[3];
						tri2[2]=puntoscara[0];
						float normal[3];
						int cid=output->InsertNextCell(VTK_TRIANGLE,3,tri1);
						vtkPolygon::ComputeNormal(puntos,3,tri1,normal);
						//normales->InsertTuple(cid,normal);
						cid=output->InsertNextCell(VTK_TRIANGLE,3,tri2);
						vtkPolygon::ComputeNormal(puntos,3,tri2,normal);
						//normales->InsertTuple(cid,normal);
					}else{
						tri1[0]=puntoscara[0];
						tri1[1]=puntoscara[2];
						tri1[2]=puntoscara[1];
						
						tri2[0]=puntoscara[3];
						tri2[1]=puntoscara[2];
						tri2[2]=puntoscara[0];
						float normal[3];
						int cid=output->InsertNextCell(VTK_TRIANGLE,3,tri1);
						vtkPolygon::ComputeNormal(puntos,3,tri1,normal);
						//normales->InsertTuple(cid,normal);
						cid=output->InsertNextCell(VTK_TRIANGLE,3,tri2);
						vtkPolygon::ComputeNormal(puntos,3,tri2,normal);
						//normales->InsertTuple(cid,normal);
					}
				}
			}
		}
	}
	UpdateProgress(1.0);
	mp->Delete();
}
