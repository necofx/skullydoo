/*
# $Id: TCLGlobals.cpp,v 1.1 2003/05/02 22:21:59 sebasfiorent Exp $
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

#include "TCLGlobals.h"

const std::string TCLGlobals::VARNAME_INPUT="filter_input";
const std::string TCLGlobals::VARNAME_OUTPUT="filter_output";
const std::string TCLGlobals::COMMAND_SHOW_PROGRESS="ShowProgress";
const std::string TCLGlobals::COMMAND_NATIVE_FILTER_SET_PARAM="NativeFilterSetParam";
const std::string TCLGlobals::COMMAND_NATIVE_FILTER_GET_PARAM="NativeFilterGetParam";
const std::string TCLGlobals::COMMAND_NATIVE_FILTER_CLEAR_PARAMS="NativeFilterClearParams";
const std::string TCLGlobals::COMMAND_NATIVE_FILTER_EXECUTE="NativeFilterExecute";

const std::string TCLGlobals::VARNAME_IMAGE_PLANE_XY="image_plane_xy";
const std::string TCLGlobals::VARNAME_IMAGE_PLANE_XZ="image_plane_xz";
const std::string TCLGlobals::VARNAME_IMAGE_PLANE_YZ="image_plane_yz";
const std::string TCLGlobals::VARNAME_IMAGE_WHOLE_DATA="image_whole";
const std::string TCLGlobals::VARNAME_IMAGE_LUT="image_lut";
const std::string TCLGlobals::VARNAME_IMAGE_RENDERER="image_renderer";

const std::string TCLGlobals::VARNAME_SURFACE_POLYDATA="surface_polydata";
const std::string TCLGlobals::VARNAME_SURFACE_ACTOR="surface_actor";
const std::string TCLGlobals::VARNAME_SURFACE_RENDERER="surface_renderer";


TCLGlobals::PredefinedObjectsMapType TCLGlobals::PredefinedObjectsMap;
TCLNativeFilter::Map TCLGlobals::NativeFiltersMap;
