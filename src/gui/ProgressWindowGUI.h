/*
# $Id: ProgressWindowGUI.h,v 1.1 2003/05/02 22:21:53 sebasfiorent Exp $
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

#ifndef _ProgressWindowGUI_h_
#define _ProgressWindowGUI_h_

#include "ProgressWindowGUIBase.h"
#include <vtkProcessObject.h>
#include <itkProcessObject.h>
#include <itkEventObject.h>
#include <vtkRenderWindow.h>
#include <fstream>
#include <string>
#include <map>

class ProgressWindowGUI:protected ProgressWindowGUIBase{
protected:
	ofstream log;
	ProgressWindowGUI();
	static ProgressWindowGUI* instance;
	static std::string description;
	typedef std::map<void*,std::string> MsgMapType;
	MsgMapType msgMap;
	MsgMapType iNamesMap;
public:
	static ProgressWindowGUI* Instance();

	void Observe(itk::ProcessObject* process,std::string msg,std::string instanceName);
	void Observe(vtkProcessObject* process,std::string msg,std::string instanceName);
	void Observe(vtkRenderWindow* renwin);
	void Update(itk::ProcessObject*, const itk::EventObject& event, void* clientData);
	void Update(vtkProcessObject* caller,unsigned long event,void* callData);
	void Update(vtkRenderWindow* renwin,unsigned long event);
	void doStartEvent(std::string msg);
	void doProgressEvent(float amount);
	void doEndEvent();
};



#endif
