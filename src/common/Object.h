/*
# $Id: Object.h,v 1.3 2003/05/23 19:16:48 sebasfiorent Exp $
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

#ifndef __Object_h
#define __Object_h
#ifdef _MSC_VER
	#pragma warning  ( disable : 4786 )
#endif

#include <iostream>
#include <typeinfo>

#include "SmartPointer.h"

#define _(String) String

#define NewMacro(x) \
	protected: \
	x(); \
	public: \
	typedef SmartPointer<x> Pointer; \
	static Pointer New(void) {  \
		Pointer smartPtr;  \
		x *rawPtr = new x; \
		smartPtr = rawPtr; \
		rawPtr->UnRegister(); \
		return smartPtr; \
	}

#define NewCopyMacro(x) \
	protected: \
	x(x& other); \
	static Pointer NewCopy(x::Pointer other) { \
		Pointer smartPtr; \
		x *rawPtr = new x; \
		smartPtr = rawPtr; \
		rawPtr->UnRegister(); \
		return smartPtr; \
	}

  
class Object 
{
public:
  typedef Object         Self;
  typedef SmartPointer<Self>  Pointer;
  typedef SmartPointer<const Self>  ConstPointer;
  
  virtual void Delete();

  virtual void Register() const;

  virtual void UnRegister() const;

  virtual int GetReferenceCount() const 
    {return m_ReferenceCount;}

  virtual void SetReferenceCount(int);

protected:
  Object():m_ReferenceCount(1) {}
  virtual ~Object(); 

  mutable int m_ReferenceCount;

private:
  Object(const Self&);
  void operator=(const Self&);
  
};
  
#endif
