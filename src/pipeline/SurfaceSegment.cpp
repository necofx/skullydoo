/*
# $Id: SurfaceSegment.cpp,v 1.3 2003/05/23 19:18:59 sebasfiorent Exp $
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

#include "SurfaceSegment.h"

#include <vtkDataSetMapper.h>
#include <vtkTextureMapToSphere.h>
#include <vtkPolyDataMapper.h>
#include <vtkTexture.h>
#include "gui/ProgressWindowGUI.h"


SurfaceSegment::SurfaceSegment():PipelineSegment(){
	surfaceActor=vtkActor::New();
	outline=vtkOutlineFilter::New();
	aoutline=vtkActor::New();
	vtkPolyDataMapper* mapper=vtkPolyDataMapper::New();
	mapper->SetInput(outline->GetOutput());
	aoutline->SetMapper(mapper);
	aoutline->SetVisibility(false);
	mapper->Delete();
}

SurfaceSegment::~SurfaceSegment(){
	input->destroyFilterPipelines();
	getVtkRenderer()->RemoveActor(aoutline);
	getVtkRenderer()->RemoveActor(surfaceActor);
	aoutline->Delete();
	outline->Delete();
	surfaceActor->Delete();
}

void SurfaceSegment::initializeSegment(){
	//getVtkRenderer()->AddActor(aoutline);
	getVtkRenderer()->AddActor(surfaceActor);
}

void SurfaceSegment::setInput(SurfaceModel::Pointer input){
	this->input=input;
	//outline->SetInput(input->getFilteredPolyData());
	vtkPolyDataMapper* mapper=vtkPolyDataMapper::New();
	mapper->SetInput(input->getFilteredPolyData());
	mapper->ScalarVisibilityOff();
	surfaceActor->SetMapper(mapper);
	mapper->Delete();
	//
	TCLFilterPipeline::Pointer fp=input->getScenePipeline();
	fp->addPredefinedObject(TCLGlobals::VARNAME_SURFACE_POLYDATA,input->getFilteredPolyData());
	fp->addPredefinedObject(TCLGlobals::VARNAME_SURFACE_RENDERER,getVtkRenderer());
	fp->addPredefinedObject(TCLGlobals::VARNAME_SURFACE_ACTOR,surfaceActor);
}

void SurfaceSegment::setTexture(vtkImageData* texture){
	vtkTextureMapToSphere* tmap=vtkTextureMapToSphere::New();
	tmap->SetInput(input->getFilteredPolyData());
	ProgressWindowGUI::Instance()->Observe(tmap,_("Generating texture coordinates"),input->getLabel());
	vtkDataSetMapper* mapper=vtkDataSetMapper::New();
    mapper->SetInput(tmap->GetOutput());
    mapper->SetScalarModeToUseCellData();
	surfaceActor->SetMapper(mapper);
	tmap->Delete();
	mapper->Delete();
	vtkTexture* tt=vtkTexture::New();
	tt->SetInput(texture);
	tt->InterpolateOn();
	surfaceActor->SetTexture(tt);
	tt->Delete();

}

void SurfaceSegment::clearTexture(){
	vtkPolyDataMapper* mapper=vtkPolyDataMapper::New();
	mapper->SetInput(input->getFilteredPolyData());
	mapper->ScalarVisibilityOff();
	surfaceActor->SetMapper(mapper);
	mapper->Delete();
	vtkTexture* texture=vtkTexture::New();
	surfaceActor->SetTexture(texture);
	texture->Delete();
}

void SurfaceSegment::highlightSurface(bool highlight){
	aoutline->SetVisibility(highlight);
}
	
vtkActor* SurfaceSegment::getSurfaceActor(){
	return surfaceActor;
}
