/*
# $Id: TCLFilterConfigGUI.h,v 1.1 2003/05/02 22:21:53 sebasfiorent Exp $
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

#ifndef _TCLFilterConfigGUI_h_
#define _TCLFilterConfigGUI_h_

#include "TCLFilterConfigGUIBase.h"
#include "tcl/TCLFilter.h"
#include "tcl/TCLFilterContinuousParameter.h"
#include "tcl/TCLFilterDiscreteParameter.h"
#include "tcl/TCLFilterStringParameter.h"

#include <vector>

class TCLFilterConfigGUI:public TCLFilterConfigGUIBase{
protected:
	TCLFilter::Pointer filter;
	void buildEditWidget(TCLFilterContinuousParameter::Pointer parameter);
	void buildEditWidget(TCLFilterDiscreteParameter::Pointer parameter);
	void buildEditWidget(TCLFilterStringParameter::Pointer parameter);	
	void buildEditWidgets();
	typedef std::vector<Fl_Widget*> WidgetVector;
	std::vector<char*> labels;
	WidgetVector widContinuous;
	WidgetVector widDiscrete;
	WidgetVector widString;
	char* makeLabel(std::string l);
public:
	TCLFilterConfigGUI(TCLFilter::Pointer filter);
	virtual ~TCLFilterConfigGUI();
	bool configure();
};

#endif
