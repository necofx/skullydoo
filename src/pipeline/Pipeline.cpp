/*
# $Id: Pipeline.cpp,v 1.1 2003/05/02 22:21:58 sebasfiorent Exp $
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

#include "Pipeline.h"
#include <algorithm>

Pipeline::Pipeline():Object(){
}

Pipeline::~Pipeline(){
	volumes.clear();
	VolumeToSegmentMap.clear();
	SurfaceToSegmentMap.clear();
}

void Pipeline::addVolume(ImageModel::Pointer volume,vtkRenderWindow* renwin){
	volumes.push_back(volume);
	ImageSegment::Pointer ss=ImageSegment::New();
	ss->setVtkRenderWindow(renwin);
	ss->setVtkRenderer(vtkRenderer::New());
	ss->setInput(volume);
	VolumeToSegmentMap[volume.GetPointer()]=ss;
}

void Pipeline::removeVolume(ImageModel::Pointer volume){
	ImageModel::Vector::iterator f=std::find(volumes.begin(),volumes.end(),volume);
	if (f!=volumes.end()){
		VolumeToSegmentMap.erase(volume.GetPointer());
		volumes.erase(f);
	}
}

ImageSegment::Pointer Pipeline::getImageSegment(ImageModel::Pointer vol){
	return VolumeToSegmentMap[vol.GetPointer()];
}

void Pipeline::addSurface(SurfaceModel::Pointer surface,vtkRenderer* renderer){
	surfaces.push_back(surface);
	SurfaceSegment::Pointer ss=SurfaceSegment::New();
	ss->setVtkRenderer(renderer);
	ss->setInput(surface);
	SurfaceToSegmentMap[surface.GetPointer()]=ss;
}

void Pipeline::removeSurface(SurfaceModel::Pointer surface){
	SurfaceModel::Vector::iterator f=std::find(surfaces.begin(),surfaces.end(),surface);
	if (f!=surfaces.end()){
		surfaces.erase(f);
		SurfaceToSegmentMap.erase(surface.GetPointer());
	}
}

SurfaceSegment::Pointer Pipeline::getSurfaceSegment(SurfaceModel::Pointer surface){
	return SurfaceToSegmentMap[surface.GetPointer()];
}
