/*
# $Id: GradientGrowthFilter.cpp,v 1.1 2003/05/02 22:21:59 sebasfiorent Exp $
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

#include "GradientGrowthFilter.h"

#include "tcl/TCL.h"
#include "gui/DesktopGUI.h"
#include "gui/ProgressWindowGUI.h"

GradientGrowthFilter::GradientGrowthFilter(){
	lastoutput=0;
}

GradientGrowthFilter::~GradientGrowthFilter(){
	if (lastoutput) lastoutput->Delete();
	FilterMapType::iterator i=filtermap.begin();
	while (i!=filtermap.end()){
		i->second->Delete();
		i++;
	}
	filtermap.clear();
}

void GradientGrowthFilter::execute(){
	TCL* tcl=TCL::Instance();
	vtkObject* io=tcl->getVtkVariable(TCLGlobals::VARNAME_INPUT);
	vtkPolyData* input=vtkPolyData::SafeDownCast(io);
	if (!io) return;
	std::string imagename=getParameter("imagename");
	float gradientlimit=atof(getParameter("gradientlimit").c_str());
	int maxiterations=atoi(getParameter("maxiterations").c_str());
	float meandistance=atof(getParameter("meandistance").c_str());
	if (imagename=="" || gradientlimit==0) return;
	ImageModel::Pointer imgpointer=DesktopGUI::Instance()->getNamedImage(imagename);
	if (!imgpointer.GetPointer()) return;
	vtkPolyData* temp=vtkPolyData::New();

	GradientGrowth* filter=filtermap[input];
	if (!filter){
		filter=new GradientGrowth();
		filter->SetInput(input);
		filtermap[input]=filter;
		ProgressWindowGUI::Instance()->Observe(filter,"Inflando por limite de gradiente","unknown");
	}
	filter->setInputGradientImage(imgpointer->getFilteredVtkVolume());
	filter->setGradientLimit(gradientlimit);
	filter->setMaxIterations(maxiterations);
	filter->setMeanDistance(meandistance);
	tcl->setVtkVariable(TCLGlobals::VARNAME_OUTPUT,filter->GetOutput());
}
