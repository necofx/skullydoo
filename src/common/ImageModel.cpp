/*
# $Id: ImageModel.cpp,v 1.1 2003/05/02 22:21:52 sebasfiorent Exp $
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

#include  "ImageModel.h"
#include <vtkStructuredPoints.h>
#include <vtkWindowLevelLookupTable.h>

ImageModel::ImageModel(){
	inputvolume=vtkStructuredPoints::New();
	label="SinNombre";
	scenepipeline=TCLFilterPipeline::New();
	scenepipeline->setInputOutputType("ImageScene","ImageScene");
	lut=vtkLookupTable::New();
	setLUTGamma(0,0,16);
	fp=TCLImageToImageFilterPipeline::New();
	fp->setInputOutputType("ImageData","ImageData");
	fp->setInput(inputvolume);
	fp->execute();
	vtk2itk=VtkItk::New();
	vtk2itk->setInput(getFilteredVtkVolume());
	itk2vtk=ItkVtk::New();
}

ImageModel::~ImageModel(){
	destroyFilterPipelines();
	lut->Delete();
	inputvolume->ReleaseData();
	inputvolume->Delete();
}

void ImageModel::destroyFilterPipelines(){
	if (scenepipeline.GetPointer()){
		scenepipeline->enablePredefinedObjects();
		scenepipeline->setFilters(TCLFilter::Vector());
		scenepipeline->disablePredefinedObjects();
		scenepipeline=0;
	}
	if (fp.GetPointer()){
		fp->enablePredefinedObjects();
		fp->setFilters(TCLFilter::Vector());
		fp->disablePredefinedObjects();
		fp=0;
	}
}

float ImageModel::interpolateColor(float* pos,vtkImageData* image,int componentNum){
  int subid;
  float weights[8];
  float pcoords[3];
  float result=0;
  int cellid=image->FindCell(pos,0,0,0,subid,pcoords,weights);
  vtkDataArray* scalars=image->GetPointData()->GetScalars();
  if (cellid>=0){
    vtkCell* celda=image->GetCell(cellid);
    for (int i=0;i<8;i++)
		result+=scalars->GetComponent(celda->GetPointId(i),componentNum)*weights[i];	
  }
  return result;
}

void ImageModel::setLabel(std::string label){
	this->label=label;
}

std::string ImageModel::getLabel(){
	return label;
}

vtkImageData* ImageModel::getInputVtkVolume(){
	return inputvolume;
}

vtkImageData* ImageModel::getFilteredVtkVolume(){
	return (vtkImageData*)fp->getOutput();
}

VtkItk::itkImageType* ImageModel::getFilteredItkVolume(){
	return vtk2itk->getOutput();
}

void ImageModel::setInputItkVolume(ItkVtk::itkImageType* input){
	itk2vtk->setup(input,inputvolume);
}

TCLFilterPipeline::Pointer ImageModel::getFiltersPipeline(){
	return fp.GetPointer();
}

TCLFilterPipeline::Pointer ImageModel::getScenePipeline(){
	return scenepipeline;
}

vtkLookupTable* ImageModel::getLookupTable(){
	return lut;
}

void ImageModel::setLUTGamma(int gamma,int startColor,int numOfCols){
	lutGamma=gamma;
	float aux;
	int i;
	int first=startColor;
	int last=startColor+numOfCols;
	lut->Allocate(numOfCols);
	lut->SetRange(first,last);
	lut->SetNumberOfColors(numOfCols);
	switch(gamma){
	case 0:
		//  #voy del negro al blanco derecho
		for (i=first;i<=last;i++) {
			float val=(float)(i-first)/numOfCols;
			lut->SetTableValue(i,val,val,val,1.0);
		}
		break;
	case 1:
		//  #voy del rojo al cyan
		for (i=first;i<=last;i++) {
			float val=(float)(i-first)/numOfCols;
			lut->SetTableValue(i,(1-val),val,val,1.0);
		}
		break;
	case 2:
		aux=numOfCols/3;
		//  # negro/rojo/majenta/blanco
		for (i=0;i<aux;i++) {
			lut->SetTableValue(first+i,(float)(i)/aux,0,0,1.0);
		}
		//	#voy del rojo al magenta
		for (i=0;i<aux;i++) {
			lut->SetTableValue(first+i+aux,1,0,(float)(i)/aux,1.0);
		}
		//  #voy del cyan al blanco
		for (int i=0;i<aux;i++) {
			lut->SetTableValue(first+i+aux*2,1,(float)(i)/aux,1,1.0);
		}
		break;
	case 3:
		aux=numOfCols/3;
		//# negro/rojo/amarillo/blanco
		// #voy hasta el negro al rojo
		for (i=0;i<aux;i++) {
			lut->SetTableValue(first+i,(float)(i)/aux,0,0,1.0);
		}
		//  #voy del rojo al amarillo
		for (i=0;i<aux;i++) {
			lut->SetTableValue(first+i+aux,1,(float)(i)/aux,0,1.0);
		}
		
		//#voy del amarillo al blanco
		for (i=0;i<aux;i++) {
			lut->SetTableValue(first+i+aux*2,1,1,(float)(i)/aux,1.0);
		}
		break;
	case 4:
		aux=numOfCols/3;
		//#negro/rojo/azul/blanco
		//  #voy hasta el negro al rojo
		for (i=0;i<aux;i++) {
			lut->SetTableValue(first+i,(float)(i)/aux,0,0,1.0);
		}
		// #voy del rojo al azul
		for (i=0;i<aux;i++) {
			lut->SetTableValue(first+i+aux,1-(float)(i)/aux,0,(float)(i)/aux,1.0);
		}
		//  #voy del azul al blanco
		for (i=0;i<aux;i++) {
			lut->SetTableValue(first+i+aux*2,(float)(i)/aux,(float)(i)/aux,1,1.0);
		}
		break;
	case 5:
		aux=numOfCols/3;
		//#negro/azul/cyan/blanco
		//  #voy hasta el negro al azul
		for (i=0;i<aux;i++) {
			lut->SetTableValue(first+i,0,0,(float)(i)/aux,1.0);
		}
		//  #voy del azul al cyan
		for (i=0;i<aux;i++) {
			lut->SetTableValue(first+i+aux,0,(float)(i)/aux,1,1.0);
		}
		//  #voy del cyan al blanco
		for (i=0;i<aux;i++) {
			lut->SetTableValue(first+i+aux*2,(float)(i)/aux,1,1,1.0);
		}
		break;
	case 6:
		aux=numOfCols/5;
		//#negro/azul/cian/verde/amarillo/blanco
		// #voy hasta el negro al azul
		for (i=0;i<aux;i++) {
			lut->SetTableValue(first+i,0,0,(float)(i)/aux,1.0);
		}
		
		//#voy del azul al cyan
		for (i=0;i<aux;i++) {
			lut->SetTableValue(first+i+aux,0,(float)(i)/aux,1,1.0);
		}
		
		//#voy del cyan al verde
		for (i=0;i<aux;i++) {
			lut->SetTableValue(first+i+aux*2,0,1,1-(float)(i)/aux,1.0);
		}
		
		//#voy del verde al amarillo
		for (i=0;i<aux;i++) {
			lut->SetTableValue(first+i+aux*3,(float)(i)/aux,1,0,1.0);
		}
		
		//#voy del amarillo al blanco
		for (i=0;i<aux;i++) {
			lut->SetTableValue(first+i+aux*4,1,1,(float)(i)/aux,1.0);
		}
		break;
	case 7:
		lut->Allocate(numOfCols);
		lut->SetRange(first,last);
		lut->SetHueRange(0,0.66667);
		lut->SetSaturationRange(1,1);
		lut->SetValueRange(1.0,1.0);
		break;
	}
	lut->Build();
}

int ImageModel::getLUTGamma(){
	return lutGamma;
}
