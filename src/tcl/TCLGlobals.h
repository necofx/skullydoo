/*
# $Id: TCLGlobals.h,v 1.2 2003/05/23 19:18:59 sebasfiorent Exp $
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

#ifndef _TCLGlobals_h_
#define _TCLGlobals_h_
#ifdef _MSC_VER
	#pragma warning  ( disable : 4786 )
#endif

#include <string>
#include <map>
#include <vtkObject.h>
#include "common/Types.h"
#include "tcl/TCLNativeFilter.h"
#include "tcl/TCLFilterPipeline.h"

class TCLGlobals{
public:
	typedef std::map<std::string,vtkObject*> PredefinedObjectsMapType;

	const static std::string VARNAME_IMAGE_PLANE_XY;
	const static std::string VARNAME_IMAGE_PLANE_XZ;
	const static std::string VARNAME_IMAGE_PLANE_YZ;
	const static std::string VARNAME_IMAGE_WHOLE_DATA;
	const static std::string VARNAME_IMAGE_LUT;
	const static std::string VARNAME_IMAGE_RENDERER;

	const static std::string VARNAME_SURFACE_POLYDATA;
	const static std::string VARNAME_SURFACE_ACTOR;
	const static std::string VARNAME_SURFACE_RENDERER;

	const static std::string VARNAME_INPUT;
	const static std::string VARNAME_OUTPUT;
	
	const static std::string COMMAND_SHOW_PROGRESS;
	const static std::string COMMAND_NATIVE_FILTER_SET_PARAM;
	const static std::string COMMAND_NATIVE_FILTER_GET_PARAM;
	const static std::string COMMAND_NATIVE_FILTER_CLEAR_PARAMS;
	const static std::string COMMAND_NATIVE_FILTER_EXECUTE;

	static PredefinedObjectsMapType PredefinedObjectsMap;
	static TCLNativeFilter::Map NativeFiltersMap;
};

#endif
