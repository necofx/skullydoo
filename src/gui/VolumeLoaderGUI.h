/*
# $Id: VolumeLoaderGUI.h,v 1.1 2003/05/02 22:21:53 sebasfiorent Exp $
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

#ifndef _VolumeLoaderGUI_h_
#define _VolumeLoaderGUI_h_

#include "VolumeLoaderGUIBase.h"
#include "common/ImageModel.h"
#include <vtkStructuredPoints.h>
#include <string>

class VolumeLoaderGUI:protected VolumeLoaderGUIBase{
protected:
	std::string filename;
	unsigned long filesize;
	enum FILE_TYPES {FORMAT_BMP,FORMAT_JPG,FORMAT_TIFF,FORMAT_PNG,FORMAT_PPM,FORMAT_RAW,FORMAT_VOL16,FORMAT_HUMAN_PROJECT};

	void clearGuessedDimensions();
	bool isCorrectDimension(int gx,int gy,int gz);
	void transform2DVolTo3DVol(vtkImageData* input,vtkImageData* output);
	
	virtual void updateSample();
	virtual void assignGuessed();
	virtual void guessDimensions();
	virtual void invalidateGuessedDims();
	virtual void fileFormatChanged();
public:
	VolumeLoaderGUI(std::string filename);
	virtual ~VolumeLoaderGUI();
	
	bool execute();
	ImageModel::Pointer readVolume();
};


#endif
