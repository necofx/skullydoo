/*
# $Id: Application.cpp,v 1.1 2003/05/02 22:21:58 sebasfiorent Exp $
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

#include "Application.h"

#include <itkExceptionObject.h>
#include <iostream>
#include <tcl.h>
#include "tcl/TCL.h"
#include "tcl/TCLGlobals.h"
#include <FL/fl_ask.H>
#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include "gui/DesktopGUI.h"

void hideSplash(void* d){
	Fl_Window* w=(Fl_Window*)d;
	w->hide();	
	delete w;
}


int Tcl_AppInit(Tcl_Interp *interp){
	int result;
	result=TCL::InitializeTCLStub(interp);
	if (result==TCL_OK){
		TCLGlobals::PredefinedObjectsMap[TCLGlobals::VARNAME_INPUT]=0;
		TCLGlobals::PredefinedObjectsMap[TCLGlobals::VARNAME_OUTPUT]=0;

		//Creo y muestro el destktop
		DesktopGUI* dg=DesktopGUI::Instance();
		dg->exec();

		Fl_Window* splash=makeSplashScreen();
		int sw=(Fl::w()-splash->w())/2;
		int sh=(Fl::h()-splash->h())/2;
		splash->position(sw,sh);
		Fl::add_timeout(3,(Fl_Timeout_Handler)hideSplash,splash);
		splash->show();
		Fl::run();
		TCLGlobals::PredefinedObjectsMap.clear();
	}
	else{
		fl_alert(Tcl_GetStringResult(interp));
	}
	Tcl_Exit(0);
    return TCL_OK;
}

int main(int argc,char** argv){
	Fl::scheme("none");
	Tcl_Main(argc, argv, Tcl_AppInit);
	return 0;
}	
