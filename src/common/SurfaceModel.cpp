/*
# $Id: SurfaceModel.cpp,v 1.2 2003/05/23 19:16:48 sebasfiorent Exp $
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

#include "SurfaceModel.h"

SurfaceModel::SurfaceModel():Object(){
	inputPolys=vtkPolyData::New();
	label=_("Unnamed");
	scenepipeline=TCLFilterPipeline::New();
	scenepipeline->setInputOutputType("SurfaceScene","SurfaceScene");
	fp=TCLPolyDataToPolyDataFilterPipeline::New();
	fp->setInputOutputType("PolyData","PolyData");
	fp->setInput(inputPolys);
	fp->execute();
	
}

SurfaceModel::~SurfaceModel(){
	destroyFilterPipelines();
	inputPolys->ReleaseData();
	inputPolys->Delete();
}

void SurfaceModel::destroyFilterPipelines(){
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

vtkPolyData* SurfaceModel::getInputPolyData(){
	return inputPolys;
}

vtkPolyData* SurfaceModel::getFilteredPolyData(){
	return (vtkPolyData*)fp->getOutput();
}

TCLFilterPipeline::Pointer SurfaceModel::getFilterPipeline(){
	return fp.GetPointer();
}

TCLFilterPipeline::Pointer SurfaceModel::getScenePipeline(){
	return scenepipeline;
}

void SurfaceModel::setLabel(std::string label){
	this->label=label;
}

std::string SurfaceModel::getLabel(){
	return label;
}

int SurfaceModel::getTriangleCount(){
	return getFilteredPolyData()->GetNumberOfCells();
}
