/*
# $Id: ImageSegment.cpp,v 1.3 2003/05/23 19:18:59 sebasfiorent Exp $
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

#include "ImageSegment.h"

#include <vtkPlaneSource.h>
#include <vtkTexture.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>

ImageSegment::ImageSegment():PipelineSegment(){
	visible=false;
	//XY
	eXY=vtkExtractVOI::New();
	textureXY=vtkTexture::New();
	textureXY->SetInterpolate(false);
	textureXY->MapColorScalarsThroughLookupTableOn();
	textureXY->SetInput(eXY->GetOutput());
	pXY=vtkPlaneSource::New();
	pXY->SetResolution(1,1);
	pXY->SetPoint1(1,0,0);
	pXY->SetPoint2(0,1,0);
	pXY->SetOrigin(0,0,0);
	aXY=vtkActor::New();
	vtkPolyDataMapper* mapper;
	mapper=vtkPolyDataMapper::New();
	mapper->SetInput(pXY->GetOutput());
	mapper->ImmediateModeRenderingOn();
	aXY->SetMapper(mapper);
	aXY->SetTexture(textureXY);
	mapper->Delete();
	pXY->Delete();
	textureXY->Delete();
	eXY->Delete();
	//XZ
	eXZ=vtkExtractVOI::New();
	textureXZ=vtkTexture::New();
	textureXZ->SetInterpolate(false);
	textureXZ->MapColorScalarsThroughLookupTableOn();
	textureXZ->SetInput(eXZ->GetOutput());
	pXZ=vtkPlaneSource::New();
	pXZ->SetResolution(1,1);
	pXZ->SetPoint1(1,0,0);
	pXZ->SetPoint2(0,0,1);
	pXZ->SetOrigin(0,0,0);
	aXZ=vtkActor::New();
	mapper=vtkPolyDataMapper::New();
	mapper->SetInput(pXZ->GetOutput());
	mapper->ImmediateModeRenderingOn();
	aXZ->SetMapper(mapper);
	aXZ->SetTexture(textureXZ);
	mapper->Delete();
	pXZ->Delete();
	textureXZ->Delete();
	//YZ
	eYZ=vtkExtractVOI::New();
	textureYZ=vtkTexture::New();
	textureYZ->SetInterpolate(false);
	textureYZ->MapColorScalarsThroughLookupTableOn();
	textureYZ->SetInput(eYZ->GetOutput());
	pYZ=vtkPlaneSource::New();
	pYZ->SetResolution(1,1);
	pYZ->SetPoint1(0,1,0);
	pYZ->SetPoint2(0,0,1);
	pYZ->SetOrigin(0,0,0);
	aYZ=vtkActor::New();
	mapper=vtkPolyDataMapper::New();
	mapper->SetInput(pYZ->GetOutput());
	mapper->ImmediateModeRenderingOn();
	aYZ->SetMapper(mapper);
	aYZ->SetTexture(textureYZ);
	mapper->Delete();
	pYZ->Delete();
	textureYZ->Delete();
	eYZ->Delete();

	voxelPos=vtkCursor3D::New();
	voxelPos->OutlineOn();
	voxelPos->AxesOn();
	voxelPos->XShadowsOn();
	voxelPos->YShadowsOn();
	voxelPos->ZShadowsOn();
	mapper=vtkPolyDataMapper::New();
	mapper->SetInput(voxelPos->GetOutput());
	aVoxelPos=vtkActor::New();
	aVoxelPos->SetMapper(mapper);
	aVoxelPos->GetProperty()->SetColor(1.0,0,0);
	mapper->Delete();

	scalarBar=vtkScalarBarActor::New();
    scalarBar->SetTitle("Intensidades");
    scalarBar->GetPositionCoordinate()->SetCoordinateSystemToNormalizedViewport();
    scalarBar->GetPositionCoordinate()->SetValue(0.1,0.01);
    scalarBar->SetOrientationToHorizontal();
    scalarBar->SetWidth(0.8);
    scalarBar->SetHeight(0.17);

	contourXY=vtkMarchingSquares::New();
	contourXY->SetInput(eXY->GetOutput());
	mapper=vtkPolyDataMapper::New();
	mapper->SetInput(contourXY->GetOutput());
	aContourXY=vtkActor::New();
	aContourXY->SetMapper(mapper);
	aContourXY->GetProperty()->SetColor(1.0,1.0,0);
	aContourXY->VisibilityOff();
	mapper->Delete();
	contourXY->Delete();

	contourXZ=vtkMarchingSquares::New();
	contourXZ->SetInput(eXZ->GetOutput());
	mapper=vtkPolyDataMapper::New();
	mapper->SetInput(contourXZ->GetOutput());
	aContourXZ=vtkActor::New();
	aContourXZ->SetMapper(mapper);
	aContourXZ->GetProperty()->SetColor(1.0,1.0,0);
	aContourXZ->VisibilityOff();
	mapper->Delete();
	contourXZ->Delete();

	contourYZ=vtkMarchingSquares::New();
	contourYZ->SetInput(eYZ->GetOutput());
	mapper=vtkPolyDataMapper::New();
	mapper->SetInput(contourYZ->GetOutput());
	aContourYZ=vtkActor::New();
	aContourYZ->SetMapper(mapper);
	aContourYZ->GetProperty()->SetColor(1.0,1.0,0);
	aContourYZ->VisibilityOff();
	mapper->Delete();
	contourYZ->Delete();

}

ImageSegment::~ImageSegment(){
	input->destroyFilterPipelines();
	aXY->Delete();
	aXZ->Delete();
	aYZ->Delete();
	aContourXY->Delete();
	aContourXZ->Delete();
	aContourYZ->Delete();
	aVoxelPos->Delete();
	scalarBar->Delete();
	getVtkRenderer()->RemoveActor(aXY);
	getVtkRenderer()->RemoveActor(aXZ);
	getVtkRenderer()->RemoveActor(aYZ);
	getVtkRenderer()->RemoveActor(aVoxelPos);
	getVtkRenderer()->RemoveActor2D(scalarBar);
	getVtkRenderer()->RemoveActor(aContourXY);
	getVtkRenderer()->RemoveActor(aContourXZ);
	getVtkRenderer()->RemoveActor(aContourYZ);
	if (visible)getVtkRenderWindow()->RemoveRenderer(getVtkRenderer());
	getVtkRenderer()->Delete();
	input=0;
}

void ImageSegment::initializeSegment(){
	getVtkRenderer()->AddActor(aXY);
	getVtkRenderer()->AddActor(aXZ);
	getVtkRenderer()->AddActor(aYZ);
	getVtkRenderer()->AddActor(aVoxelPos);
	getVtkRenderer()->AddActor2D(scalarBar);
	getVtkRenderer()->AddActor(aContourXY);
	getVtkRenderer()->AddActor(aContourXZ);
	getVtkRenderer()->AddActor(aContourYZ);
	visible=true;
}

void ImageSegment::setInput(ImageModel::Pointer input){
	this->input=input;
	inputLUTChanged();
	eXY->SetInput(input->getFilteredVtkVolume());
	eXZ->SetInput(input->getFilteredVtkVolume());
	eYZ->SetInput(input->getFilteredVtkVolume());
	int dims[3];
	int extents[6];
	float bounds[6];
	input->getFilteredVtkVolume()->Update();
	input->getFilteredVtkVolume()->GetDimensions(dims);
	input->getFilteredVtkVolume()->GetBounds(bounds);
	input->getFilteredVtkVolume()->GetExtent(extents);
	voxelPos->SetModelBounds(bounds);
	VoxelModel initSeed;
	initSeed.x=(dims[0]/2)+extents[0];
	initSeed.y=(dims[1]/2)+extents[2];
	initSeed.z=(dims[2]/2)+extents[4];
	focusSeed(initSeed);
	//
	TCLFilterPipeline::Pointer fp=input->getScenePipeline();
	fp->addPredefinedObject(TCLGlobals::VARNAME_IMAGE_PLANE_XY,eXY->GetOutput());
	fp->addPredefinedObject(TCLGlobals::VARNAME_IMAGE_PLANE_XZ,eXZ->GetOutput());
	fp->addPredefinedObject(TCLGlobals::VARNAME_IMAGE_PLANE_YZ,eYZ->GetOutput());
	fp->addPredefinedObject(TCLGlobals::VARNAME_IMAGE_WHOLE_DATA,input->getFilteredVtkVolume());
	fp->addPredefinedObject(TCLGlobals::VARNAME_IMAGE_RENDERER,getVtkRenderer());
}

void ImageSegment::inputLUTChanged(){
	vtkLookupTable* lut=input->getLookupTable();
	if (!lut) return;
	scalarBar->SetLookupTable(lut);
	TCLFilterPipeline::Pointer fp=input->getScenePipeline();
	fp->addPredefinedObject(TCLGlobals::VARNAME_IMAGE_LUT,lut);
	textureXY->SetLookupTable(lut);
	textureXZ->SetLookupTable(lut);
	textureYZ->SetLookupTable(lut);
	aXY->Modified();
	aXZ->Modified();
	aYZ->Modified();
}

void ImageSegment::focusSeed(VoxelModel focus){
	seedFocus=focus;
	float bounds[6];
	float origin[3];
	float spacing[3];
	int dims[3];
	int extents[6];
	eXY->GetInput()->GetSpacing(spacing);
	eXY->GetInput()->GetBounds(bounds);
	eXY->GetInput()->GetOrigin(origin);
	eXY->GetInput()->GetExtent(extents);
	eXY->GetInput()->GetDimensions(dims);

	voxelPos->SetFocalPoint(origin[0]+(focus.x*spacing[0]),origin[1]+(focus.y*spacing[1]),origin[2]+(focus.z*spacing[2]));

	eXY->SetVOI(0,dims[0]-1,0,dims[1]-1,focus.z-extents[4],focus.z-extents[4]);
	eXY->Update();
	pXY->SetPoint1(bounds[1],bounds[2],origin[2]+(focus.z*spacing[2]));
	pXY->SetPoint2(bounds[0],bounds[3],origin[2]+(focus.z*spacing[2]));
	pXY->SetOrigin(bounds[0],bounds[2],origin[2]+(focus.z*spacing[2]));

	eXZ->SetVOI(0,dims[0]-1,focus.y-extents[2],focus.y-extents[2],0,dims[2]-1);
	eXZ->Update();
	pXZ->SetPoint1(bounds[1],origin[1]+(focus.y*spacing[1]),bounds[4]);
	pXZ->SetPoint2(bounds[0],origin[1]+(focus.y*spacing[1]),bounds[5]);
	pXZ->SetOrigin(bounds[0],origin[1]+(focus.y*spacing[1]),bounds[4]);

	eYZ->SetVOI(focus.x-extents[0],focus.x-extents[2],0,dims[1]-1,0,dims[2]-1);
	eYZ->Update();
	pYZ->SetPoint1(origin[0]+(focus.x*spacing[0]),bounds[3],bounds[4]);
	pYZ->SetPoint2(origin[0]+(focus.x*spacing[0]),bounds[2],bounds[5]);
	pYZ->SetOrigin(origin[0]+(focus.x*spacing[0]),bounds[2],bounds[4]);

	aXY->Modified();
	aXZ->Modified();
	aYZ->Modified();
}

VoxelModel ImageSegment::getFocusedSeed(){
	return seedFocus;
}

void ImageSegment::setVisibility(bool visibility){
	visible=visibility;
	if (!visibility)
		getVtkRenderWindow()->RemoveRenderer(getVtkRenderer());
	else
		getVtkRenderWindow()->AddRenderer(getVtkRenderer());
}

bool ImageSegment::getVisibility(){
	return visible;
}

void ImageSegment::setPlaneVisibility(PlaneType plane,bool visibility){
	switch(plane){
	case PLANE_XY:
		aXY->SetVisibility(visibility);
		break;
	case PLANE_XZ:
		aXZ->SetVisibility(visibility);
		break;
	case PLANE_YZ:
		aYZ->SetVisibility(visibility);
		break;
	}
}

bool ImageSegment::getPlaneVisibility(PlaneType plane){
	switch(plane) {
	case PLANE_XY:
		return aXY->GetVisibility();
		break;
	case PLANE_XZ:
		return aXZ->GetVisibility();
		break;
	default:
		return aYZ->GetVisibility();
	}
}

void ImageSegment::setLUTBarVisibility(bool show){
	if (show) scalarBar->VisibilityOn();
	else
		scalarBar->VisibilityOff();
}

bool ImageSegment::getLUTBarVisibility(){
	return scalarBar->GetVisibility();
}

void ImageSegment::setContourVisibility(bool show){
	if (show){
		contourXY->Update();
		aContourXY->VisibilityOn();
		contourXZ->Update();
		aContourXZ->VisibilityOn();
		contourYZ->Update();
		aContourYZ->VisibilityOn();
	}
	else{
		aContourXY->VisibilityOff();
		aContourXZ->VisibilityOff();
		aContourYZ->VisibilityOff();
	}
}

bool ImageSegment::getContourVisibility(){
	return aContourXY->GetVisibility();
}

void ImageSegment::setContourValue(float value){
	contourXY->SetValue(0,value);
	contourXZ->SetValue(0,value);
	contourYZ->SetValue(0,value);
}
