/*
# $Id: FastMarchingMethod.h,v 1.2 2003/05/23 19:18:59 sebasfiorent Exp $
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

#ifndef _FastMarchingMethod_h_
#define _FastMarchingMethod_h_
#ifdef _MSC_VER
	#pragma warning  ( disable : 4786 )
#endif

#include "common/Object.h"
#include "common/Types.h"

#include <itkImage.h>
#include <itkCovariantVector.h>
#include <itkGradientRecursiveGaussianImageFilter.h>
#include <itkFastMarchingImageFilter.h>
#include <itkImageRegionIterator.h>
#include <vnl/vnl_math.h>

class FastMarchingMethod:public Object{
public:
	typedef itk::Image<ITK_VoxelType,3> itkImageType;
	typedef itk::CovariantVector<ITK_VoxelType,3> VectorType;
	typedef itk::Image<VectorType,3> DerivativeImageType;
	typedef itk::GradientRecursiveGaussianImageFilter<itkImageType,DerivativeImageType>  DerivativeFilterType;
	typedef itk::FastMarchingImageFilter<itkImageType,itkImageType> DetectionFilterType;
protected:
	typedef DetectionFilterType::NodeType NodeType;
	typedef DetectionFilterType::NodeContainer NodeContainer;
	NodeContainer::Pointer trialPoints;
	itkImageType::Pointer input;
	itkImageType::Pointer edge_potential;
	DetectionFilterType::Pointer detection_filter;
	double sigma;
	double threshold;
	void computeEdgePotentialMap();
	void computeTimeCrossingMap();
public:
	NewMacro(FastMarchingMethod);

	void setInput(itkImageType* input);
	void setSeed(int identifier,itkImageType::IndexType seed);
	void setSigma(double sigma);
	itkImageType* getOutput();
};

#endif
