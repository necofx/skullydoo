/*
# $Id: Config.cpp,v 1.1 2003/05/02 22:21:52 sebasfiorent Exp $
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

#include "Config.h"
#include <FL/filename.H>
#include <FL/fl_ask.H>

#define ELEM_FILTERDIR "FilterDirectory"

Config* Config::instance=0;
const std::string Config::CONFIG_FILENAME="./config.xml";

Config::Config(){
	char path[1024];
	fl_filename_absolute(path,1024,CONFIG_FILENAME.c_str());
	if (!dom.LoadFile(CONFIG_FILENAME.c_str())){
		std::string xml="<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>\n"\
						"<Config>\n"\
						"<FilterDirectory>./filters/</FilterDirectory>\n"\
						"</Config>\n";
		dom.Parse(xml.c_str());
	}
	root=dom.FirstChildElement();
}

Config::~Config(){
}

void Config::save(){
	dom.SaveFile(CONFIG_FILENAME);
}

Config* Config::Instance(){
	if (!instance) instance=new Config();
	return instance;
}

std::string Config::getFiltersDirectory(){
	return root->FirstChildElement(ELEM_FILTERDIR)->FirstChild()->Value();
}

void Config::setFiltersDirectory(std::string dir){
	root->FirstChildElement(ELEM_FILTERDIR)->FirstChild()->SetValue(dir);
}
