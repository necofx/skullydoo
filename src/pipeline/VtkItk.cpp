/*
# $Id: VtkItk.cpp,v 1.1 2003/05/02 22:21:59 sebasfiorent Exp $
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

#include "VtkItk.h"
#include "gui/ProgressWindowGUI.h"

template <typename itkImporter>
void connectVtkToItkPipeline(vtkImageExport* exporter, itkImporter importer){
	importer->SetUpdateInformationCallback(exporter->GetUpdateInformationCallback());
	importer->SetPipelineModifiedCallback(exporter->GetPipelineModifiedCallback());
	importer->SetWholeExtentCallback(exporter->GetWholeExtentCallback());
	importer->SetSpacingCallback(exporter->GetSpacingCallback());
	importer->SetOriginCallback(exporter->GetOriginCallback());
	importer->SetScalarTypeCallback(exporter->GetScalarTypeCallback());
	importer->SetNumberOfComponentsCallback(exporter->GetNumberOfComponentsCallback());
	importer->SetPropagateUpdateExtentCallback(exporter->GetPropagateUpdateExtentCallback());
	importer->SetUpdateDataCallback(exporter->GetUpdateDataCallback());
	importer->SetDataExtentCallback(exporter->GetDataExtentCallback());
	importer->SetBufferPointerCallback(exporter->GetBufferPointerCallback());
	importer->SetCallbackUserData(exporter->GetCallbackUserData());
}

VtkItk::VtkItk(){
	vtkImageExporter=vtkImageExport::New();
	itkImageImporter=itkVtkVolumeImporterType::New();
	ProgressWindowGUI::Instance()->Observe(itkImageImporter,"Conectando pipelines VTK->ITK","unknown");
	connectVtkToItkPipeline(vtkImageExporter,itkImageImporter);
}

VtkItk::~VtkItk(){
	vtkImageExporter->Delete();
}

VtkItk::itkImageType* VtkItk::getOutput(){
	const float* spacing=vtkImageExporter->GetInput()->GetSpacing();
	const float* origin=vtkImageExporter->GetInput()->GetOrigin();
	output->SetSpacing(spacing);
	output->SetOrigin(origin);
	return output;
}

void VtkItk::setInput(vtkImageData* input){
	vtkImageExporter->SetInput(input);
	output=itkImageImporter->GetOutput();
}
