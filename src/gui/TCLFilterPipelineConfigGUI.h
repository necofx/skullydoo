/*
# $Id: TCLFilterPipelineConfigGUI.h,v 1.1 2003/05/02 22:21:53 sebasfiorent Exp $
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

#ifndef _TCLFilterPipelineConfigGUI_h_
#define _TCLFilterPipelineConfigGUI_h_

#include "TCLFilterPipelineConfigGUIBase.h"
#include "tcl/TCLFilterPipeline.h"


class TCLFilterPipelineConfigGUI:protected TCLFilterPipelineConfigGUIBase{
protected:
	bool needReExecution;
	char labelholder[1024];
	TCLFilterPipeline::Pointer filterpipeline;
	TCLFilter::Vector filters;
	TCLFilter::Vector available;

	TCLFilter::Pointer getSelectedFilter();
	void updateCorrectConfigurationStatus();
	void updateAvailableFiltersCombo(std::string inputtype);
	
	virtual void filterSelected();
	virtual void availableFilterSelected();
	virtual void addFilter();
	virtual void removeFilter();
	virtual void moveEarlier();
	virtual void moveLater();
	virtual void configureFilter();
	
public:
	TCLFilterPipelineConfigGUI(TCLFilterPipeline::Pointer filterpipeline,TCLFilter::Vector available);
	//True si ejecuto ok
	bool configure();
};

#endif
