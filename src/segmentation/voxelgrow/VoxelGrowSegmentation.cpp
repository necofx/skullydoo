/*
# $Id: VoxelGrowSegmentation.cpp,v 1.4 2005/05/09 16:20:41 nacholarrabide Exp $
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

#include "VoxelGrowSegmentation.h"
#include "vtkBMPWriter.h"
#include "vtkImageCast.h"

#include "gui/ProgressWindowGUI.h"
#include "common/ImageModel.h"
#include "segmentation/voxelgrow/Healer.h"
#include "FlatContour.h"
#include <vtkWindowedSincPolyDataFilter.h>


#include "segmentation/topologicalderivative/TDMethod.h"

VoxelGrowSegmentation::VoxelGrowSegmentation():SegmentationMethod(){
}

VoxelGrowSegmentation::~VoxelGrowSegmentation(){
}

void VoxelGrowSegmentation::setRadius(int radius){
	this->radius=radius;
}

void VoxelGrowSegmentation::setStretchRadius(int stretchradius){
	this->stretchradius=stretchradius;
}

void VoxelGrowSegmentation::execute(ImageModel::Pointer vol,Segmentation::Pointer result){
/**/	VoxelGrow* vg=new VoxelGrow();
	vg->SetInput(vol->getFilteredVtkVolume());
	vg->setRadius(radius);
	vg->setStretchRadius(stretchradius);
	vg->setConfig(config);
	ProgressWindowGUI::Instance()->Observe(vg,_("Executing VoxelGrow Segmentation"),vol->getLabel());
	// Removed beacuse it´s dubious utility
	Healer *h= new Healer();
	h->SetInput(vg->GetOutput());
	h->SetOutput(result->getImage()->getInputVtkVolume());
	ProgressWindowGUI::Instance()->Observe(h,_("Eliminando casos patologicos"),vol->getLabel());
	h->Update();
	vtkImageData* fake=vtkImageData::New();
	h->SetOutput(fake);
	//vg->SetOutput(fake);
	//vg->Update();
	fake->Delete();
	vg->Delete();
	result->setHaveImage(true);
	h->Delete();
/*	*/
/*	TDMethod* tder = new TDMethod();
	//	TDMethod* tder=TDMethod::New();
	tder->SetInput(vol->getFilteredVtkVolume());
	TDMethod::ClassesVector classes;
	classes.push_back(0.0);
	classes.push_back(100.0);
	tder->setClasses(classes);
	tder->setRho(0.5);
	int activeDirections[3];
	//segmentate in the xy direction
	activeDirections[0] = activeDirections[1] = 1;
	activeDirections[2] = 0;
	tder->setActiveDirections(activeDirections);
	tder->SetOutput(result->getImage()->getInputVtkVolume());
	ProgressWindowGUI::Instance()->Observe(tder,_("Executing Topological Derivative Segmentation"),vol->getLabel());
	tder->Update();
	vtkImageData* fake=vtkImageData::New();
	tder->SetOutput(fake);
	//vg->SetOutput(fake);
	//vg->Update();
	fake->Delete();
	result->setHaveImage(true);
	//	tder->Delete();
*/
}

void VoxelGrowSegmentation::setConfig(VoxelGrow::ConfigVector config){
	this->config=config;
}
