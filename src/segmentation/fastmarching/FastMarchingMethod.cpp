/*
# $Id: FastMarchingMethod.cpp,v 1.2 2003/05/06 00:12:14 sebasfiorent Exp $
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

#include "FastMarchingMethod.h"

#include "itkImageRegionIterator.h"
#include "itkExceptionObject.h"
#include "itkFastMarchingImageFilter.h"
#include <math.h>
#include "gui/ProgressWindowGUI.h"

FastMarchingMethod::FastMarchingMethod():Object(){
	edge_potential=itkImageType::New();
	detection_filter=DetectionFilterType::New();
	sigma=1.0;
	trialPoints=NodeContainer::New();
}

void FastMarchingMethod::setInput(itkImageType* input){
	this->input=input;
}

void FastMarchingMethod::setSeed(int identifier,itkImageType::IndexType seed){
  NodeType node;
  node.SetValue( 0.0 );
  node.SetIndex( seed);
  trialPoints->InsertElement(identifier,node);
}

void FastMarchingMethod::setSigma(double sigma){
	this->sigma=sigma;
}


void FastMarchingMethod::computeTimeCrossingMap(){
  // connect edge potential map
  detection_filter->SetInput(edge_potential);
  detection_filter->SetTrialPoints( trialPoints );
  // specify the size of the output image
  detection_filter->SetOutputSize( input->GetBufferedRegion().GetSize() );
  // update the marcher
  ProgressWindowGUI::Instance()->Observe(detection_filter,"Haciendo FastMarching...","unknown");
  detection_filter->Update();
}

void FastMarchingMethod::computeEdgePotentialMap(){
	DerivativeFilterType::Pointer deriv = DerivativeFilterType::New();
	deriv->SetInput(input);
	deriv->SetSigma(sigma);
	ProgressWindowGUI::Instance()->Observe(deriv,"Calculando mapa de potenciales de fast marching","unknown");
	deriv->Update();
	
	edge_potential->SetLargestPossibleRegion(input->GetLargestPossibleRegion());
	edge_potential->SetBufferedRegion(input->GetBufferedRegion());
	edge_potential->Allocate();
	
	typedef itk::ImageRegionIterator<DerivativeImageType> DerivativeIterator;
	typedef itk::ImageRegionIterator<itkImageType>ImageIterator;
	
	DerivativeIterator derivIter( deriv->GetOutput(),deriv->GetOutput()->GetBufferedRegion() );
	ImageIterator mapIter( edge_potential,edge_potential->GetBufferedRegion() );
	
	while( !derivIter.IsAtEnd() ){
		
		float accum = 0;
		VectorType grad = derivIter.Get();
		
		for( int j = 0; j < 3; j++ ){
			accum += vnl_math_sqr( grad[j] );
		}
		
		accum = sqrt( accum );
		//    mapIter.Set( 1.0 / ( 1.0 + accum ) );
		mapIter.Set( exp( -1.0 * accum ) );

		++derivIter;
		++mapIter;

    }
}

FastMarchingMethod::itkImageType* FastMarchingMethod::getOutput(){
	computeEdgePotentialMap();
	computeTimeCrossingMap();
	return detection_filter->GetOutput();
}


