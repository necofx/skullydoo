/*
# $Id: ActiveContourMethod.cpp,v 1.2 2004/06/07 22:59:49 nacholarrabide Exp $
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

#include "ActiveContourMethod.h"

#include <itkCovariantVector.h>
#include <itkGradientImageFilter.h>
#include <itkGradientRecursiveGaussianImageFilter.h>
#include <itkLaplacianImageFilter.h>
#include <itkGradientVectorFlowImageFilter.h>
#include <itkGradientToMagnitudeImageFilter.h>
#include <itkBinaryMask3DMeshSource.h>
#include <itkDeformableMesh3DFilter.h>
#include <itkImageBase.h>

ActiveContourMethod::ActiveContourMethod(){
}

void ActiveContourMethod::setInput(ImageType* input){
	this->input=input;
}

void ActiveContourMethod::setBinaryImage(BinaryImageType* binaryImage,unsigned short objectvalue){
	this->binaryImage=binaryImage;
	this->binaryObjectValue=objectvalue;
}

void ActiveContourMethod::setGradientNoiseLevel(double level){
	this->gradientNoiseLevel=level;
}

ActiveContourMethod::DMeshType::Pointer ActiveContourMethod::getOutput(){
	typedef itk::CovariantVector<double,3> GradientType;
	typedef itk::CovariantVector<int,3> int3DVector;
	typedef itk::CovariantVector<double,3> double3DVector;
	typedef itk::CovariantVector<double,2> double2DVector;
	typedef itk::Image<GradientType,3> GradientImageType;
	typedef itk::GradientRecursiveGaussianImageFilter<ImageType,GradientImageType> GradientRecursiveFilterType;
	typedef itk::LaplacianImageFilter<ImageType,ImageType> LaplacianFilterType;
	typedef itk::GradientVectorFlowImageFilter<GradientImageType,GradientImageType> GVFFilterType;
    typedef itk::GradientImageFilter<ImageType, double, double> GradientFilterType;
	typedef itk::GradientToMagnitudeImageFilter<GradientImageType,ImageType> GToMFilterType;
	typedef itk::BinaryMask3DMeshSource<DMeshType>  MeshSourceType;
    typedef itk::DeformableMesh3DFilter<DMeshType, DMeshType>  DFilterType;

	GradientFilterType::Pointer gfilter=GradientFilterType::New();
	gfilter->SetInput(input);

	//con GVF
	LaplacianFilterType::Pointer lpfilter=LaplacianFilterType::New();
	GVFFilterType::Pointer gvffilter=GVFFilterType::New();
	gvffilter->SetInput(gfilter->GetOutput());
	gvffilter->SetLaplacianFilter(lpfilter);
	gvffilter->SetNoiseLevel(gradientNoiseLevel);
	gvffilter->Update();

	//con gradiente comun
	GradientRecursiveFilterType::Pointer grfilter=GradientRecursiveFilterType::New();
	GToMFilterType::Pointer gtomfilter = GToMFilterType::New();
	grfilter->SetInput(input);
	grfilter->SetSigma(2.0);//2da derivada siempre
	gtomfilter->SetInput(grfilter->GetOutput());
	gfilter->SetInput(gtomfilter->GetOutput());
	//gfilter->Update();
	//Malla deformable
	MeshSourceType::Pointer meshsource = MeshSourceType::New();
	DFilterType::Pointer dfilter = DFilterType::New();
	dfilter->SetInput(meshsource->GetOutput());
	ImageType::IndexType center;
	center[0]=0;center[1]=0;center[2]=0;
	//dfilter->SetCenter(center);
	dfilter->SetGradient(gfilter->GetOutput());
	dfilter->SetStepThreshold(100);   // based on he application, usually less than 150
	meshsource->SetBinaryImage(binaryImage);
	meshsource->SetObjectValue(binaryObjectValue);
	int3DVector resolution;
	resolution[0]=100;
	resolution[1]=100;
	resolution[2]=100;
	//dfilter->SetResolution(resolution);
	double2DVector m_stiff;
	m_stiff[0] = 0.00001;
	m_stiff[1] = 0.04;
	dfilter->SetStiffness(m_stiff);
	dfilter->SetTimeStep(0.01);         // Typical value: 0.01
	const itk::ImageBase<3>::SpacingType spacing=input->GetSpacing();
	double3DVector m_scale;
	m_scale[0] = spacing[0];
	m_scale[1] = spacing[1];
	m_scale[2] = spacing[2];
//	m_scale[0] = 1;
//	m_scale[1] = 1;
//	m_scale[2] = 1;
	dfilter->SetScale(m_scale);
	dfilter->Update();
	return dfilter->GetOutput();
}
