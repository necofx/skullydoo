/*
# $Id: SegmentationMethod.cpp,v 1.2 2005/05/09 16:20:41 nacholarrabide Exp $
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

#include "SegmentationMethod.h"
#include "pipeline/VtkItk.h"
#include "pipeline/ItkVtk.h"

SegmentationMethod::SegmentationMethod():Object(){
}

SegmentationMethod::~SegmentationMethod(){
}

void SegmentationMethod::initialize(ImageModel::Pointer vol,Segmentation::Pointer result){
	result->setStartExecutionTime();
}

void SegmentationMethod::execute(ImageModel::Pointer vol,Segmentation::Pointer result){
}

void SegmentationMethod::finalize(ImageModel::Pointer vol,Segmentation::Pointer result){
	result->setEndExecutionTime();
}

Segmentation::Pointer SegmentationMethod::segmentate(ImageModel::Pointer vol){
	Segmentation::Pointer result=Segmentation::New();
	initialize(vol,result);
	execute(vol,result);
	finalize(vol,result);
	return result;
}
