/*
# $Id: TCLFilter.cpp,v 1.1 2003/05/02 22:21:59 sebasfiorent Exp $
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

#include "TCLFilter.h"
#include "TCLFilterDiscreteParameter.h"
#include "TCLFilterContinuousParameter.h"
#include "TCLFilterStringParameter.h"
#include "TCL.h"
#include "TCLGlobals.h"

#include <vector>
#include <iostream>

TCLFilter::TCLFilter():Object(){
	initialized=false;
	finalized=false;
}

void TCLFilter::setFileName(std::string filename){
	this->filename=filename;
	if (dom.LoadFile(filename)){
		root=dom.FirstChildElement("TCLFilter");
		loadParameters();
	}
	else
		root=0;
}

TCLFilter::~TCLFilter(){
	finalize();
}

void TCLFilter::finalize(){
	if (!finalized && initialized){
		TCL* tcl=TCL::Instance();
		VarsMapType::iterator i;
		//Renombro las variables temporales
		for (i=tclVarsMap.begin();i!=tclVarsMap.end();i++){
			std::string varname=i->first;
			std::string tempname=i->second;
			tcl->renameVtkVariable(tempname,varname);
		}
		bool ok=tcl->evaluate(getFinishTclCode());
		std::string errormsg=tcl->getLastError();
		for (i=tclVarsMap.begin();i!=tclVarsMap.end();i++){
			std::string varname=i->first;
			std::string tempname=i->second;
			tcl->clearVariable(varname);
		}
		tclVarsMap.clear();
		finalized=true;
	}
}

void TCLFilter::loadParameters(){
	parameters.clear();
	TiXmlElement* param=root->FirstChild("Parameters")->FirstChildElement();
	while (param){
		std::string val=param->Value();
		if (val=="Discrete_Parameter"){
			TCLFilterDiscreteParameter::Pointer fp=TCLFilterDiscreteParameter::New();
			fp->setXmlRoot(param);
			parameters.push_back(fp.GetPointer());
		}
		if (val=="Continuous_Parameter"){
			TCLFilterContinuousParameter::Pointer fp=TCLFilterContinuousParameter::New();
			fp->setXmlRoot(param);
			parameters.push_back(fp.GetPointer());
		}
		if (val=="String_Parameter"){
			TCLFilterStringParameter::Pointer fp=TCLFilterStringParameter::New();
			fp->setXmlRoot(param);
			parameters.push_back(fp.GetPointer());
		}
		param=param->NextSiblingElement();
	}
}

TCLFilterParameter::Vector TCLFilter::getParameters(){
	return parameters;
}

bool TCLFilter::existParameter(std::string name){
	TCLFilterParameter::Vector v=getParameters();
	for (TCLFilterParameter::Vector::iterator i=v.begin();i!=v.end();i++){
		TCLFilterParameter::Pointer p=(*i);
		if (p->getName()==name) return true;
	}
	return false;
}

TCLFilterParameter::Pointer TCLFilter::getParameter(std::string name){
	TCLFilterParameter::Vector v=getParameters();
	for (TCLFilterParameter::Vector::iterator i=v.begin();i!=v.end();i++){
		TCLFilterParameter::Pointer p=(*i);
		if (p->getName()==name) return p;
	}
	return 0;
}

std::string TCLFilter::getFileName(){
	return filename;
}

std::string TCLFilter::getInitTclCode(){
	std::string result="";
	TiXmlNode* cdata=root->FirstChild("InitTCLCode")->FirstChild();
	if (cdata && cdata->Value()){
		result=cdata->Value();
		result=result.substr(std::string("![CDATA[").length());
		result=result.substr(0,result.length()-std::string("]]").length());
	}
	return result;
}

std::string TCLFilter::getTclMarquedCode(){
	std::string result="";
	TiXmlNode* cdata=root->FirstChild("TCLCode")->FirstChild();
	if (cdata && cdata->Value()){
		result=cdata->Value();
		result=result.substr(std::string("![CDATA[").length());
		result=result.substr(0,result.length()-std::string("]]").length());
	}
	return result;
}

std::string TCLFilter::getTclParsedCode(){
	std::string tclCode="";
	bool ok=true;
	std::string aux=getTclMarquedCode();
	std::string varName;
	std::string varValue;
	while (ok && aux.find("%%")!=std::string::npos) {
		tclCode.append(aux.substr(0,aux.find_first_of("%%")));
		aux.erase(0,aux.find_first_of("%%")+2);
		varName=aux;
		varName.erase(aux.find_first_of("%%"),varName.length()-1);
		aux.erase(0,aux.find_first_of("%%")+2);
		if (existParameter(varName)) varValue=getParameter(varName)->getValue();
		else
			ok=false;
		tclCode.append(varValue);
	}
	tclCode.append(aux);		
	if (!ok) return "";
	return tclCode;
}

std::string TCLFilter::getFinishTclCode(){
	std::string result="";
	TiXmlNode* cdata=root->FirstChild("FinishTCLCode");
	if (cdata) cdata=cdata->FirstChild();
	if (cdata && cdata->Value()){
		result=cdata->Value();
		result=result.substr(std::string("![CDATA[").length());
		result=result.substr(0,result.length()-std::string("]]").length());
	}
	return result;
}


std::string TCLFilter::getInputType(){
	TiXmlElement* e=root->FirstChildElement("INPUT_Plug_Type");
	std::string result="";
	if (e){
		if (e->Attribute("Input_Type")) result=e->Attribute("Input_Type");
	}
	return result;
}

std::string TCLFilter::getOutputType(){
	TiXmlElement* e=root->FirstChildElement("OUTPUT_Plug_Type");
	std::string result="";
	if (e){
		if (e->Attribute("Output_Type")) result=e->Attribute("Output_Type");
	}
	return result;
}

std::string TCLFilter::getName(){
	return root->Attribute("Name");
}

std::string TCLFilter::getMappedName(std::string varname){
	if (tclVarsMap.find(varname)==tclVarsMap.end()){
		return varname;
	}
	return tclVarsMap[varname];
}

bool TCLFilter::initialize(){
	bool ok=false;
	if (!initialized){
		initialized=true;
		tclVarsMap.clear();
		TCL* tcl=TCL::Instance();
		ok=tcl->evaluate(getInitTclCode());
		if (!ok) executionError=tcl->getLastError();
		//Obtengo todas las variables vtk creadas en esta etapa
		std::vector<std::string> vtkObjects=tcl->getVtkObjectNamesInInterpreter();
		for (std::vector<std::string>::iterator i=vtkObjects.begin();i!=vtkObjects.end();i++){
			std::string varname=(*i);
			//Si fue exitoso, renombre, sino, borro
			TCLGlobals::PredefinedObjectsMapType::iterator i=TCLGlobals::PredefinedObjectsMap.find(varname);
			if (i==TCLGlobals::PredefinedObjectsMap.end() &&
				varname.find("vtkTemp")!=0 &&
				varname.find("filterTemp")!=0){
				if (ok){
					std::string tempname=tcl->createNewFilterVariableName();
					tclVarsMap[varname]=tempname;//Renombro esta variable por un nombre temporal		
					tcl->renameVtkVariable(varname,tempname);
				}
				else
					tcl->clearVariable(varname);
			}
		}
	}
	return ok;	
}

bool TCLFilter::execute(){
	executionError="";
	TCL* tcl=TCL::Instance();
	VarsMapType::iterator i;
	//Renombro las variables temporales
	for (i=tclVarsMap.begin();i!=tclVarsMap.end();i++){
		std::string varname=i->first;
		std::string tempname=i->second;
		tcl->renameVtkVariable(tempname,varname);
	}
	bool ok=tcl->evaluate(getTclParsedCode());
	if (!ok) executionError=tcl->getLastError();
	//Renombro otra vez a temporales
	//Renombro las variables temporales
	for (i=tclVarsMap.begin();i!=tclVarsMap.end();i++){
		std::string varname=i->first;
		std::string tempname=i->second;
		tcl->renameVtkVariable(varname,tempname);
	}
	return ok;
}

std::string TCLFilter::getTCLError(){
	return executionError;
}
