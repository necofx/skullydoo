/*
# $Id: WeakPointer.h,v 1.2 2003/05/23 18:50:39 sebasfiorent Exp $
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

#ifndef __WeakPointer_h
#define __WeakPointer_h
#ifdef _MSC_VER
	#pragma warning  ( disable : 4786 )
#endif

#include <iostream>

template <class TObjectType>
class WeakPointer {
public:
	typedef TObjectType ObjectType;
	
	WeakPointer () 
    { m_Pointer = 0; }
	
	WeakPointer (const WeakPointer<ObjectType> &p):
    m_Pointer(p.m_Pointer) { }
	
	WeakPointer (ObjectType *p):
    m_Pointer(p) { }                             
	
	~WeakPointer ()
    { m_Pointer = 0; }
	
	ObjectType *operator -> () const
    { return m_Pointer; }
	
	operator ObjectType * () const 
    { return m_Pointer; }
	
	template <typename R>
		bool operator == (R r) const
    {
		return (m_Pointer == (ObjectType*)r);
    }
	template <typename R>
		bool operator != (R r) const
    {
		return (m_Pointer != (ObjectType*)r);
    }
    
	ObjectType *GetPointer () const 
    { return m_Pointer; }
	
	bool operator < (const WeakPointer &r)
    { return (void*)m_Pointer < (void*) r.m_Pointer; }
	
	bool operator > (const WeakPointer &r)
    { return (void*)m_Pointer > (void*) r.m_Pointer; }
	
	bool operator <= (const WeakPointer &r)
    { return (void*)m_Pointer <= (void*) r.m_Pointer; }
	
	bool operator >= (const WeakPointer &r)
    { return (void*)m_Pointer >= (void*) r.m_Pointer; }
	
	WeakPointer &operator = (const WeakPointer &r)
    { return this->operator = (r.GetPointer()); }
	
	WeakPointer &operator = (ObjectType *r)
	{
		m_Pointer = r; 
		return *this;
    }
	
private:
	ObjectType* m_Pointer;
};  


#endif
