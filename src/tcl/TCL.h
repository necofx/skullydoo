/*
# $Id: TCL.h,v 1.1 2003/05/02 22:21:59 sebasfiorent Exp $
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

#ifndef _TCL_h_
#define _TCL_h_

#include <tcl.h>
#include <vtkObject.h>
#include <string>
#include <vector>

class TCL{
protected:
	int tempVarCount;
	std::string lastError;
	Tcl_Interp* interpreter;
	static TCL* instance;
	TCL();
public:
	static int InitializeTCLStub(Tcl_Interp* interp);

	static TCL* Instance();
	Tcl_Interp* getInterpreter();

	bool evaluate(std::string code);
	std::string getLastError();

	std::vector<std::string> getVtkObjectNamesInInterpreter();
	bool existsVariable(std::string varname);
	std::string getVariable(std::string varname);
	void setVariable(std::string varname,std::string value);
	void clearVariable(std::string varname);
	void setVtkVariable(std::string varname,vtkObject* variable);
	vtkObject* getVtkVariable(std::string varname);
	void renameVtkVariable(std::string oldname,std::string newname);
	std::string createNewFilterVariableName();
};

#endif
