/*
# $Id: VoxelProcessor.h,v 1.2 2003/05/23 18:52:51 sebasfiorent Exp $
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

#ifndef _VoxelProcessor_h_
#define _VoxelProcessor_h_
#ifdef _MSC_VER
	#pragma warning  ( disable : 4786 )
#endif

#include "common/Object.h"
#include "common/VoxelModel.h"
#include <vtkImageData.h>

class VoxelProcessor:public Object{
protected:
	vtkImageData* input;
	int extent[6];
	float delta;
	VoxelModel seed;
public:
	NewMacro(VoxelProcessor);
	virtual ~VoxelProcessor();
	
	virtual void setDelta(float delta);
	float getDelta();
	virtual void setSeed(VoxelModel seed);
	virtual void setInputImage(vtkImageData* img);
	virtual void initialize();
	virtual bool processNeighbor(VoxelModel point,VoxelModel neigh);
	virtual void finalize();
};

#endif
