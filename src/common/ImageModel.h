/*
# $Id: ImageModel.h,v 1.1 2003/05/02 22:21:52 sebasfiorent Exp $
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

#ifndef _ImageModel_h_
#define _ImageModel_h_

#include "Object.h"
#include "pipeline/VtkItk.h"
#include "pipeline/ItkVtk.h"
#include "tcl/TCLImageToImageFilterPipeline.h"
#include <vtkImageData.h>
#include <vtkLookupTable.h>
#include <string>
#include <vector>

class ImageModel:public Object{
protected:
	std::string label;
	int lutGamma;
	vtkImageData* inputvolume;
	VtkItk::Pointer vtk2itk;
	ItkVtk::Pointer itk2vtk;
	vtkLookupTable* lut;
	TCLFilterPipeline::Pointer scenepipeline;
	TCLImageToImageFilterPipeline::Pointer fp;
public:
	NewMacro(ImageModel);
	virtual ~ImageModel();
	typedef std::vector<Pointer> Vector;

	static float interpolateColor(float* pos,vtkImageData* image,int componentNum);

	void destroyFilterPipelines();
	void setLabel(std::string label);
	std::string getLabel();
	TCLFilterPipeline::Pointer getFiltersPipeline();
	TCLFilterPipeline::Pointer getScenePipeline();
	vtkImageData* getInputVtkVolume();
	vtkImageData* getFilteredVtkVolume();
	void setInputItkVolume(ItkVtk::itkImageType* input);
	VtkItk::itkImageType* getFilteredItkVolume();
	
	vtkLookupTable* getLookupTable();
	void setLUTGamma(int gamma,int startColor,int numOfCols);
	int getLUTGamma();

	
};	

#endif
