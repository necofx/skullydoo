/*
# $Id: TCLNativeFilter.h,v 1.1 2003/05/02 22:21:59 sebasfiorent Exp $
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

#ifndef _TCLNativeFilter_h_
#define _TCLNativeFilter_h_

#include "common/Object.h"
#include <vector>
#include <string>
#include <map>

class TCLNativeFilter:public Object{
protected:
	typedef std::map<std::string,std::string> StringMapType;
	StringMapType vars;
public:
	NewMacro(TCLNativeFilter);
	typedef std::vector<Pointer> Vector;
	typedef std::map<std::string,Pointer> Map;

	virtual void setParameter(std::string name,std::string value);
	virtual std::string getParameter(std::string name);
	virtual void clearParameters();
	virtual void execute();
};

#endif
