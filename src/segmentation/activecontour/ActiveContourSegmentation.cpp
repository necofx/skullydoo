/*
# $Id: ActiveContourSegmentation.cpp,v 1.1 2003/05/02 22:21:59 sebasfiorent Exp $
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

#include "ActiveContourSegmentation.h"
#include "ActiveContourMethod.h"
#include <itkCastImageFilter.h>
#include "pipeline/ItkVtk.h"

ActiveContourSegmentation::ActiveContourSegmentation(){
}

ActiveContourSegmentation::~ActiveContourSegmentation(){
}

void ActiveContourSegmentation::setBinaryImage(ImageModel::Pointer binaryimg,int objval){
	this->binaryImg=binaryimg;
	this->binaryObjVal=objval;
}

void ActiveContourSegmentation::execute(ImageModel::Pointer vol,Segmentation::Pointer result){
	typedef itk::Image<double,3> Double3DImageType;
	typedef itk::Image<unsigned short,3> UShort3DImageType;
	typedef itk::CastImageFilter<ItkVtk::itkImageType,Double3DImageType> ToDoubleCasterType;
	typedef itk::CastImageFilter<ItkVtk::itkImageType,UShort3DImageType> ToUShortCasterType;
	
	ToDoubleCasterType::Pointer doubleCaster=ToDoubleCasterType::New();
	doubleCaster->SetInput(vol->getFilteredItkVolume());
    doubleCaster->Update();	
	ToUShortCasterType::Pointer ushortCaster=ToUShortCasterType::New();
	ushortCaster->SetInput(binaryImg->getFilteredItkVolume());
	ushortCaster->Update();

	ActiveContourMethod::Pointer acm=ActiveContourMethod::New();
	acm->setInput(doubleCaster->GetOutput());
	acm->setBinaryImage(ushortCaster->GetOutput(),binaryObjVal);
	acm->setGradientNoiseLevel(500);
	ItkVtk::Pointer itkvtk=ItkVtk::New();
	itkvtk->setup(acm->getOutput(),result->getSurface()->getInputPolyData());
	result->setHaveSurface(true);
}


