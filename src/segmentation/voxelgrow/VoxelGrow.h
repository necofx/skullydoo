/*
# $Id: VoxelGrow.h,v 1.1 2003/05/02 22:21:59 sebasfiorent Exp $
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

#ifndef _VoxelGrowH_
#define _VoxelGrowH_

#include "common/VoxelModel.h"
#include "VoxelProcessor.h"

#include <vtkImageData.h>
#include <vtkPointSet.h>
#include <vtkSimpleImageToImageFilter.h>
#include <vtkBitArray.h>
#include <map>
#include <vector>

class VoxelGrow:public vtkSimpleImageToImageFilter{
public:
	typedef std::pair<VoxelModel,VoxelProcessor::Pointer> ConfigPair;
	typedef std::vector<ConfigPair> ConfigVector;
	
	static float VALUE_REGION_OUT;

	VoxelGrow();
	virtual ~VoxelGrow();
	
	void setConfig(ConfigVector config);
	void setRadius(int radius);
	void setStretchRadius(int stretchradius);
	ConfigVector getConfig();
protected:
	int radius;
	int stretchradius;
	ConfigVector config;
	vtkBitArray* stretchedPoints;


	virtual void SimpleExecute(vtkImageData* input,vtkImageData* output);
};

#endif

