/*
# $Id: Segmentation.cpp,v 1.1 2003/05/02 22:21:59 sebasfiorent Exp $
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

#include "Segmentation.h"

#include <time.h>

Segmentation::Segmentation():Object(){
	starttime=clock();
	stoptime=starttime;
	label="unnamed";
	image=ImageModel::New();
	surface=SurfaceModel::New();
	haveimg=false;
	havesrf=false;
}

void Segmentation::setStartExecutionTime(){
	starttime=clock();
}

void Segmentation::setEndExecutionTime(){
	stoptime=clock();
}

unsigned long Segmentation::getExecutionTimeInMillis(){
	unsigned long diff=stoptime-starttime;	
	return (diff*1000)/CLK_TCK;
}

void Segmentation::setLabel(std::string label){
	this->label=label;
}

std::string Segmentation::getLabel(){
	return label;
}

bool Segmentation::haveImage(){
	return haveimg;
}

bool Segmentation::haveSurface(){
	return havesrf;
}

void Segmentation::setHaveImage(bool have){
	haveimg=have;
}

void Segmentation::setHaveSurface(bool have){
	havesrf=have;
}

ImageModel* Segmentation::getImage(){
	return image.GetPointer();
}

SurfaceModel* Segmentation::getSurface(){
	return surface.GetPointer();
}
