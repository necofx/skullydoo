/*
# $Id: TCLFilterUtils.cpp,v 1.2 2003/05/06 00:12:14 sebasfiorent Exp $
# SkullyDoo - Segmentador y visualizador de imagenes tridimensionales  
# (C) 2002 Sebasti n Fiorentini / Ignacio Larrabide
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

#include "TCLFilterUtils.h"
#include <FL/filename.H>
#include <FL/fl_ask.H>
#include <string>
#include <algorithm>

TCLFilter::Vector TCLFilterUtils::hasInputType(std::string type, TCLFilter::Vector filters){
	TCLFilter::Vector res;
	std::string ftype;
	for (TCLFilter::Vector::iterator it = filters.begin();it!=filters.end();it++){
		TCLFilter::Pointer f=(*it);
		ftype = f->getInputType();
		if (ftype==type) res.push_back(f);
	}
	return res;
}

TCLFilter::Vector TCLFilterUtils::hasOutputType(std::string type, TCLFilter::Vector filters)
{
	TCLFilter::Vector res;
	std::string ftype;
	for (TCLFilter::Vector::iterator it = filters.begin();it!=filters.end();it++){
		TCLFilter::Pointer f=(*it);
		ftype = f->getOutputType();
		if (ftype==type) res.push_back(f);
	}
	return res;
}

TCLFilter::Vector TCLFilterUtils::getAvailableFilters(std::string dir){
	char path[1024];
	fl_filename_absolute(path,1024,dir.c_str());
	dirent	**files;	// Files in in directory
        int num_files = fl_filename_list(path, &files);
	TCLFilter::Vector result;
        if (num_files > 0) {
		for (int i = 0; i < num_files; i ++){
			std::string fname=files[i]->d_name;
			std::string lowercasefname=fname;
			std::transform(fname.begin(),fname.end(),lowercasefname.begin(),tolower);
			if (fl_filename_match(lowercasefname.c_str(),"*.xml")){
				TCLFilter::Pointer f=TCLFilter::New();
				f->setFileName(std::string(path)+fname);
				result.push_back(f);
			}
		}
		for (int i = num_files; i > 0;) {
			free((void*)(files[--i]));
		}
		free((void*)files);
	}
	return result;
}
