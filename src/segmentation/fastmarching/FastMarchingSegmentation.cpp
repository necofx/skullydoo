/*
# $Id: FastMarchingSegmentation.cpp,v 1.1 2003/05/02 22:21:59 sebasfiorent Exp $
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

#include "FastMarchingSegmentation.h"
#include "FastMarchingMethod.h"
#include "pipeline/ItkVtk.h"
#include "pipeline/VtkItk.h"

FastMarchingSegmentation::FastMarchingSegmentation():SegmentationMethod(){
}

void FastMarchingSegmentation::execute(ImageModel::Pointer vol,Segmentation::Pointer result){
	FastMarchingMethod::Pointer fmm=FastMarchingMethod::New();
	float orig[3];
	float spac[3];
	vol->getFilteredVtkVolume()->GetOrigin(orig);
	vol->getFilteredVtkVolume()->GetSpacing(spac);
	fmm->setInput(vol->getFilteredItkVolume());
	VoxelModel::Vector::iterator vi;
	int pos=0;
	for (vi=seeds.begin();vi!=seeds.end();vi++){
		VoxelModel seed=(*vi);
		FastMarchingMethod::itkImageType::IndexType index;
		index[0]=seed.x;
		index[1]=seed.y;
		index[2]=seed.z;
		fmm->setSeed(pos,index);
		pos++;
	}
	fmm->setSigma(sigma);
	result->getImage()->setInputItkVolume(fmm->getOutput());
	result->getImage()->getInputVtkVolume()->SetOrigin(orig);
	result->getImage()->getInputVtkVolume()->SetOrigin(spac);
	result->getImage()->getFiltersPipeline()->execute();
	result->setHaveImage(true);
}

void FastMarchingSegmentation::setSeeds(VoxelModel::Vector seeds){
	this->seeds=seeds;
}

void FastMarchingSegmentation::setSigma(float sigma){
	this->sigma=sigma;
}

