/*
# $Id: Object.cpp,v 1.1 2003/05/02 22:21:52 sebasfiorent Exp $
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

//NOTA: Este archivo fue tomado del codigo de ITK

#include "Object.h"

void Object::Delete() 
{
  this->UnRegister();
}

void Object::Register() const
{
  m_ReferenceCount++;
}


void Object::UnRegister() const{
  m_ReferenceCount--;
  
  // ReferenceCount in now unlocked.  We may have a race condition to
  // to delete the object.
  if ( m_ReferenceCount <= 0)
    {
    delete this;
    }
}


void Object::SetReferenceCount(int ref){
  m_ReferenceCount = ref;

  if ( m_ReferenceCount <= 0)
    {
    delete this;
    }
}

Object::~Object() {
  if ( m_ReferenceCount > 0)
    {
	//Debug: Esto no tendria que pasar :D
    }
}


