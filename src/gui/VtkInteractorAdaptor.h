/*
# $Id: VtkInteractorAdaptor.h,v 1.2 2003/05/23 18:50:40 sebasfiorent Exp $
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

#ifndef _VtkInteractorAdaptor_h_
#define _VtkInteractorAdaptor_h_
#ifdef _MSC_VER
	#pragma warning  ( disable : 4786 )
#endif

#include <vtkInteractorStyleUser.h>
#include "Interactor.h"

class VtkInteractorAdaptor:public vtkInteractorStyleUser{
protected:
	void OnChar(int ctrl, int shift, char keycode, int repeatcount);
	void OnKeyPress(int ctrl, int shift, char keycode, char *keysym,int repeatcount);
	void OnKeyRelease(int ctrl, int shift, char keycode, char *keysym,int repeatcount);
	void OnLeftButtonDown(int ctrl, int shift, int X, int Y);
	void OnLeftButtonUp(int ctrl, int shift, int X, int Y);
	void OnMiddleButtonDown(int ctrl, int shift, int X, int Y);
	void OnMiddleButtonUp(int ctrl, int shift, int X, int Y);
	void OnRightButtonDown(int ctrl, int shift, int X, int Y);
	void OnRightButtonUp(int ctrl, int shift, int X, int Y);
	void OnMouseMove(int ctrl, int shift, int X, int Y);
	void OnConfigure(int width, int height);
	void OnEnter(int ctrl, int shift, int X, int Y);
	void OnLeave(int ctrl, int shift, int X, int Y);
	void OnTimer(void);

	Interactor::Pointer i;

public:
	VtkInteractorAdaptor(Interactor::Pointer interactor);
};

#endif
