/*
# $Id: VtkInteractorAdaptor.cpp,v 1.1 2003/05/02 22:21:53 sebasfiorent Exp $
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

#include "VtkInteractorAdaptor.h"

VtkInteractorAdaptor::VtkInteractorAdaptor(Interactor::Pointer interactor){
	this->i=interactor;
}

void VtkInteractorAdaptor::OnChar(int ctrl, int shift, char keycode, int repeatcount){
	i->OnChar(ctrl,shift,keycode,repeatcount);
}

void VtkInteractorAdaptor::OnConfigure(int width, int height){
	i->OnConfigure(width,height);
}

void VtkInteractorAdaptor::OnEnter(int ctrl, int shift, int X, int Y){
	i->OnEnter(ctrl,shift,X,Y);
}

void VtkInteractorAdaptor::OnKeyPress(int ctrl, int shift, char keycode, char *keysym, int repeatcount){
	i->OnKeyPress(ctrl,shift,keycode,keysym,repeatcount);
}

void VtkInteractorAdaptor::OnKeyRelease(int ctrl, int shift, char keycode, char *keysym, int repeatcount){
	i->OnKeyRelease(ctrl,shift,keycode,keysym,repeatcount);
}

void VtkInteractorAdaptor::OnLeave(int ctrl, int shift, int X, int Y){
	i->OnLeave(ctrl,shift,X,Y);
}

void VtkInteractorAdaptor::OnLeftButtonDown(int ctrl, int shift, int X, int Y){
	i->OnLeftButtonDown(ctrl,shift,X,Y);
}

void VtkInteractorAdaptor::OnLeftButtonUp(int ctrl, int shift, int X, int Y){
	i->OnLeftButtonUp(ctrl,shift,X,Y);
}

void VtkInteractorAdaptor::OnMiddleButtonDown(int ctrl, int shift, int X, int Y){
	i->OnMiddleButtonDown(ctrl,shift,X,Y);
}

void VtkInteractorAdaptor::OnMiddleButtonUp(int ctrl, int shift, int X, int Y){
	i->OnMiddleButtonUp(ctrl,shift,X,Y);
}

void VtkInteractorAdaptor::OnRightButtonDown(int ctrl, int shift, int X, int Y){
	i->OnRightButtonDown(ctrl,shift,X,Y);
}

void VtkInteractorAdaptor::OnRightButtonUp(int ctrl, int shift, int X, int Y){
	i->OnRightButtonUp(ctrl,shift,X,Y);
}

void VtkInteractorAdaptor::OnMouseMove(int ctrl, int shift, int X, int Y){
	i->OnMouseMove(ctrl,shift,X,Y);
}

void VtkInteractorAdaptor::OnTimer(){
	i->OnTimer();
}
