/*
# $Id: ItkVtk.h,v 1.2 2003/05/23 19:18:59 sebasfiorent Exp $
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

#ifndef _ItkVtk_h_
#define _ItkVtk_h_
#ifdef _MSC_VER
	#pragma warning  ( disable : 4786 )
#endif

#include "common/Types.h"
#include <vtkImageImport.h>
#include <itkVTKImageExport.h>
#include "common/Object.h"
#include "common/Types.h"
#include <vtkImageData.h>
#include <vtkPolyData.h>
#include <itkImage.h>
#include <itkMesh.h>

class ItkVtk:public Object{
public:
	typedef itk::Image<ITK_VoxelType,3> itkImageType;
	typedef itk::Mesh<double,3> itkMeshType;
	typedef	itk::VTKImageExport<itkImageType> itkVtkVolumeExporterType;
protected:
	vtkImageImport* vtkImageImporter;
	itkVtkVolumeExporterType::Pointer itkImageExporter;
public:
	NewMacro(ItkVtk);
	virtual ~ItkVtk();
	void setup(itkImageType* input,vtkImageData* output);
	void setup(itkMeshType* input,vtkPolyData* output);
};


#endif
