/*
# $Id: VoxelGrowSegmentation.cpp,v 1.2 2003/05/23 18:53:58 sebasfiorent Exp $
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
	VoxelGrow* vg=new VoxelGrow();
	vg->SetInput(vol->getFilteredVtkVolume());
	vg->setRadius(radius);
	vg->setStretchRadius(stretchradius);
	vg->setConfig(config);
	ProgressWindowGUI::Instance()->Observe(vg,_("Executing VoxelGrow Segmentation"),vol->getLabel());
	/* Removed beacuse it´s dubious utility
	Healer *h= new Healer();
	h->SetInput(vg->GetOutput());
	h->SetOutput(result->getImage()->getInputVtkVolume());
	ProgressWindowGUI::Instance()->Observe(h,_("Eliminando casos patologicos"),vol->getLabel());
	h->Update();
	*/
	vtkImageData* fake=vtkImageData::New();
	//h->SetOutput(fake);
	vg->SetOutput(fake);
	fake->Delete();
	vg->Delete();
	result->setHaveImage(true);
	//h->Delete();
}

void VoxelGrowSegmentation::setConfig(VoxelGrow::ConfigVector config){
	this->config=config;
}

