/*
# $Id: TCLFilterDiscreteParameter.h,v 1.2 2003/05/23 19:18:59 sebasfiorent Exp $
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


#if !defined(_TCLFilterDiscRETEPARAMETER_H_)
#define _TCLFilterDiscRETEPARAMETER_H_
#ifdef _MSC_VER
	#pragma warning  ( disable : 4786 )
#endif

#include "TCLFilterParameter.h"
#include <string>
#include <vector>

class TCLFilterDiscreteParameter : public TCLFilterParameter  {
public:
	NewMacro(TCLFilterDiscreteParameter);
	// retorna true si el valor es valido
	bool existsValue(std::string);
	std::vector<std::string> getValues();
};

#endif // !defined(_TCLFilterDiscRETEPARAMETER_H_)
