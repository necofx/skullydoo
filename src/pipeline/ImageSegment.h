/*
# $Id: ImageSegment.h,v 1.1 2003/05/02 22:21:58 sebasfiorent Exp $
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

#ifndef _ImageSegment_h_
#define _ImageSegment_h_

#include "PipelineSegment.h"
#include "common/VoxelModel.h"
#include "common/Types.h"
#include "common/ImageModel.h"

#include <vtkExtractVOI.h>
#include <vtkPlaneSource.h>
#include <vtkImageData.h>
#include <vtkImageExport.h>
#include <vtkPolyData.h>
#include <vtkLookupTable.h>
#include <vtkActor.h>
#include <vtkCursor3D.h>
#include <vtkScalarBarActor.h>
#include <vtkMarchingSquares.h>

class ImageSegment:public PipelineSegment{
protected:
	bool visible;
	VoxelModel seedFocus;
	ImageModel::Pointer input;

	vtkExtractVOI* eXY;
	vtkExtractVOI* eXZ;
	vtkExtractVOI* eYZ;
	vtkPlaneSource* pXY;
	vtkPlaneSource* pXZ;
	vtkPlaneSource* pYZ;
	vtkCursor3D* voxelPos;
	vtkActor* aXY;
	vtkActor* aXZ;
	vtkActor* aYZ;
	vtkActor* aVoxelPos;
	vtkScalarBarActor* scalarBar;

	vtkMarchingSquares* contourXY;
	vtkMarchingSquares* contourXZ;
	vtkMarchingSquares* contourYZ;

	vtkActor* aContourXY;
	vtkActor* aContourXZ;
	vtkActor* aContourYZ;
	
	vtkTexture* textureXY;
	vtkTexture* textureXZ;
	vtkTexture* textureYZ;

	virtual void initializeSegment();
public:
	NewMacro(ImageSegment);
	virtual ~ImageSegment();

	void focusSeed(VoxelModel focus);
	VoxelModel getFocusedSeed();
	void setInput(ImageModel::Pointer input);
	void inputLUTChanged();

	void setPlaneVisibility(PlaneType plane,bool visibility);
	bool getPlaneVisibility(PlaneType plane);

	void setVisibility(bool visibility);
	bool getVisibility();

	void setLUTBarVisibility(bool show);
	bool getLUTBarVisibility();

	void setContourVisibility(bool show);
	bool getContourVisibility();

	void setContourValue(float value);
	float getContourValue();
};

#endif
