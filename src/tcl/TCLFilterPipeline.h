/*
# $Id: TCLFilterPipeline.h,v 1.2 2003/05/23 19:18:59 sebasfiorent Exp $
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

#ifndef _TCLFilterPipeline_h_
#define _TCLFilterPipeline_h_
#ifdef _MSC_VER
	#pragma warning  ( disable : 4786 )
#endif

#include "common/Object.h"
#include <vtkObject.h>
#include "TCLFilter.h"
#include "TCLGlobals.h"
#include <map>
#include <string>
#include <vector>

class TCLFilterPipeline:public Object{
protected:
	vtkObject* input;
	vtkObject* copyoutput;
	TCLFilter::Vector filters;
	std::string inputType;
	std::string outputType;
	std::string executionError;
	typedef std::map<std::string,vtkObject*> PredefinedObjectsType;
	PredefinedObjectsType predefinedObjects;

	virtual void passDataToFinalOutput(vtkObject* currentOutput);
public:
	enum ExecStatus {EXEC_OK,EXEC_ERROR,EXEC_MISSED_OUTPUT};
	NewMacro(TCLFilterPipeline);
	virtual ~TCLFilterPipeline();

	void addPredefinedObject(std::string name,vtkObject* object);
	void deletePredefinedObject(std::string name);
	void enablePredefinedObjects();
	void disablePredefinedObjects();
	
	void setInput(vtkObject* input);
	void setInputOutputType(std::string inputType,std::string outputType);
	std::string getInputType();
	std::string getOutputType();
	vtkObject* getInput();
	virtual vtkObject* getOutput();
	TCLFilter::Vector getFilters();
	void setFilters(TCLFilter::Vector filters);
	virtual ExecStatus execute();
	std::string getErrorMessage();
};

#endif
