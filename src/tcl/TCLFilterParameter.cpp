/*
# $Id: TCLFilterParameter.cpp,v 1.1 2003/05/02 22:21:59 sebasfiorent Exp $
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


#include "TCLFilterParameter.h"


TCLFilterParameter::TCLFilterParameter():Object(){
}

void TCLFilterParameter::setXmlRoot(TiXmlElement* root){
	valAssigned=false;
	this->xml=root;
	if (getDefaultValue()!="") setValue(getDefaultValue());
}

std::string TCLFilterParameter::getName(){
	std::string result="";
	if (xml->Attribute("Name")) result=xml->Attribute("Name");
	return result;
}

std::string TCLFilterParameter::getHint(){
	std::string result="";
	if (xml->Attribute("Hint")) result=xml->Attribute("Hint");
	return result;
}

std::string TCLFilterParameter::getValue(){
	return value;
}

std::string TCLFilterParameter::getDefaultValue(){
	std::string result="";
	if (xml->Attribute("DefaultValue")) result=xml->Attribute("DefaultValue");
	return result;
}

void TCLFilterParameter::setValue(std::string value){
	this->value=value;
	valAssigned=true;
}

std::string TCLFilterParameter::getType(){
	return xml->Value();
}

bool TCLFilterParameter::valueAssigned(){
	return valAssigned;
}
