/*
# $Id: Interactor.h,v 1.2 2003/05/23 19:18:58 sebasfiorent Exp $
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

#ifndef _Interactor_h_
#define _Interactor_h_
#ifdef _MSC_VER
	#pragma warning  ( disable : 4786 )
#endif

#include "common/Object.h"

class Interactor:public Object{
public:
	NewMacro(Interactor);
	
	virtual void OnChar(int ctrl, int shift, char keycode, int repeatcount){}
	virtual void OnKeyPress(int ctrl, int shift, char keycode, char *keysym,int repeatcount){}
	virtual void OnKeyRelease(int ctrl, int shift, char keycode, char *keysym,int repeatcount){}
	virtual void OnLeftButtonDown(int ctrl, int shift, int X, int Y){}
	virtual void OnLeftButtonUp(int ctrl, int shift, int X, int Y){}
	virtual void OnMiddleButtonDown(int ctrl, int shift, int X, int Y){}
	virtual void OnMiddleButtonUp(int ctrl, int shift, int X, int Y){}
	virtual void OnRightButtonDown(int ctrl, int shift, int X, int Y){}
	virtual void OnRightButtonUp(int ctrl, int shift, int X, int Y){}
	virtual void OnMouseMove(int ctrl, int shift, int X, int Y){}
	virtual void OnConfigure(int width, int height){}
	virtual void OnEnter(int ctrl, int shift, int X, int Y){}
	virtual void OnLeave(int ctrl, int shift, int X, int Y){}
	virtual void OnTimer(void){}
};

#endif
