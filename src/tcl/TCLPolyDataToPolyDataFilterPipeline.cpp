/*
# $Id: TCLPolyDataToPolyDataFilterPipeline.cpp,v 1.1 2003/05/02 22:21:59 sebasfiorent Exp $
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

#include "TCLPolyDataToPolyDataFilterPipeline.h"

PassPolyDataFilter::PassPolyDataFilter():vtkPolyDataToPolyDataFilter(){
}

void PassPolyDataFilter::Execute(){
	GetOutput()->DeepCopy(GetInput());
}

TCLPolyDataToPolyDataFilterPipeline::TCLPolyDataToPolyDataFilterPipeline():TCLFilterPipeline(){
	passFilter=new PassPolyDataFilter();
}

TCLPolyDataToPolyDataFilterPipeline::~TCLPolyDataToPolyDataFilterPipeline(){
	passFilter->ReleaseDataFlagOn();
	passFilter->Delete();
}

void TCLPolyDataToPolyDataFilterPipeline::passDataToFinalOutput(vtkObject* currenOutput){
	vtkPolyData* pd=vtkPolyData::SafeDownCast(currenOutput);
	if (pd) passFilter->SetInput(pd);
}

vtkObject* TCLPolyDataToPolyDataFilterPipeline::getOutput(){
	return passFilter->GetOutput();
}
