/*
# $Id: TCLFilterPipeline.cpp,v 1.2 2003/05/23 19:18:59 sebasfiorent Exp $
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

#include "TCLFilterPipeline.h"
#include "tcl/TCLGlobals.h"
#include "tcl/TCL.h"

#include <vtkDataSet.h>
#include <stack>

TCLFilterPipeline::TCLFilterPipeline(){
	input=0;
	copyoutput=0;
}

TCLFilterPipeline::~TCLFilterPipeline(){
	filters.clear();
}

void TCLFilterPipeline::addPredefinedObject(std::string name,vtkObject* object){
	predefinedObjects[name]=object;
	TCLGlobals::PredefinedObjectsMap[name]=object;
}

void TCLFilterPipeline::deletePredefinedObject(std::string name){
	predefinedObjects.erase(name);
	TCLGlobals::PredefinedObjectsMap.erase(name);
}

void TCLFilterPipeline::enablePredefinedObjects(){
	//Seteo variables predefinidas
	TCL* tcl=TCL::Instance();
	PredefinedObjectsType::iterator it;
	for (it=predefinedObjects.begin();it!=predefinedObjects.end();it++)
		tcl->setVtkVariable(it->first,it->second);
	//
}

void TCLFilterPipeline::disablePredefinedObjects(){
	//Elimino variables predefinidas
	TCL* tcl=TCL::Instance();
	PredefinedObjectsType::iterator it;
	for (it=predefinedObjects.begin();it!=predefinedObjects.end();it++)
		tcl->clearVariable(it->first);
	//
}

void TCLFilterPipeline::passDataToFinalOutput(vtkObject* currentOutput){
	copyoutput=currentOutput;
}

void TCLFilterPipeline::setInputOutputType(std::string inputType,std::string outputType){
	this->inputType=inputType;
	this->outputType=outputType;
}

std::string TCLFilterPipeline::getInputType(){
	return inputType;
}

void TCLFilterPipeline::setInput(vtkObject* input){
	this->input=input;
}

vtkObject* TCLFilterPipeline::getInput(){
	return input;
}

vtkObject* TCLFilterPipeline::getOutput(){
	return copyoutput;
}

std::string TCLFilterPipeline::getOutputType(){
	return outputType;
}

TCLFilter::Vector TCLFilterPipeline::getFilters(){
	return filters;
}

void TCLFilterPipeline::setFilters(TCLFilter::Vector filters){
	this->filters.clear();
	this->filters=filters;
}

TCLFilterPipeline::ExecStatus TCLFilterPipeline::execute(){
	executionError="";
	vtkObject* currentInput=input;
	vtkObject* currentOutput=0;
	TCL* tcl=TCL::Instance();
	bool ok=true;
	//Caso especial: No hay filtros 
	if (filters.empty()){
		//Si los tipos concuerdan,entonces salida<=entrada
		if (inputType==outputType){
			passDataToFinalOutput(input);
			return EXEC_OK;
		}
		executionError=_("Some filters are missing and the input/output type doesn´t match");
		return EXEC_MISSED_OUTPUT;
	}
	enablePredefinedObjects();
	for (TCLFilter::Vector::iterator i=filters.begin();ok && i!=filters.end();i++){
		TCLFilter::Pointer filter=(*i);
		tcl->setVtkVariable(TCLGlobals::VARNAME_INPUT,currentInput);
		ok=filter->execute();
		if (!ok) executionError=filter->getTCLError();;
		tcl->clearVariable(TCLGlobals::VARNAME_INPUT);
		std::string ovarname=TCLGlobals::VARNAME_OUTPUT;
		currentOutput=tcl->getVtkVariable(ovarname);
		if (!currentOutput){
			ovarname=tcl->getVariable(TCLGlobals::VARNAME_OUTPUT);
			if (ovarname!="") currentOutput=tcl->getVtkVariable(filter->getMappedName(ovarname));
		}
		tcl->clearVariable(TCLGlobals::VARNAME_OUTPUT);
		currentInput=currentOutput;
	}
	if (ok) passDataToFinalOutput(currentOutput);
	disablePredefinedObjects();
	return (ok?EXEC_OK:EXEC_ERROR);
}

std::string TCLFilterPipeline::getErrorMessage(){
	return executionError;
}
