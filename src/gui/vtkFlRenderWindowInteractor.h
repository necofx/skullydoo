/*
# $Id: vtkFlRenderWindowInteractor.h,v 1.2 2003/05/23 18:52:49 sebasfiorent Exp $
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

/*
 * vtkFlRenderWindowInteractor - class to enable VTK to render to and interact
 * with a FLTK window.
 * 
 * Copyright (c) 2002 Charl P. Botha <cpbotha@ieee.org> http://cpbotha.net/
 * Based on original code and concept copyright (c) 2000,2001 David Pont
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
 * 
 * See the .cxx for more notes.
 * 
 * $Id: vtkFlRenderWindowInteractor.h,v 1.2 2003/05/23 18:52:49 sebasfiorent Exp $
 */

#ifndef _vtkFlRenderWindowInteractor_h
#define _vtkFlRenderWindowInteractor_h
#ifdef _MSC_VER
	#pragma warning  ( disable : 4786 )
#endif

#include <FL/Fl.H>
#include <FL/Fl_Gl_Window.H>
#include <vtkRenderWindowInteractor.h>

class vtkFlRenderWindowInteractor : public Fl_Gl_Window, public vtkRenderWindowInteractor {
 protected:
   // Fl_Gl_Window overrides
   void flush(void);
   void draw( void );
   void resize( int x, int y, int w, int h );
   int  handle( int event );

 public:
   // ctors
   vtkFlRenderWindowInteractor();
   vtkFlRenderWindowInteractor( int x, int y, int w, int h, const char *l="");
   // vtk ::New()
   static vtkFlRenderWindowInteractor * New();
   // dtor
   ~vtkFlRenderWindowInteractor( void );

   // vtkRenderWindowInteractor overrides
   void Delete();
   void Initialize();
   void Enable();
   void Disable();
   void Start();
   void SetRenderWindow(vtkRenderWindow *aren);
   void UpdateSize(int x, int y);
   int CreateTimer(int timertype);
   int DestroyTimer();
   void OnTimer(void);
   void TerminateApp();
  };

#endif
