/*
# $Id: SmartPointer.h,v 1.2 2003/05/23 19:11:32 sebasfiorent Exp $
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

#ifndef __SmartPointer_h
#define __SmartPointer_h
#ifdef _MSC_VER
	#pragma warning  ( disable : 4786 )
#endif

#include "WeakPointer.h"

template <class TObjectType>
class SmartPointer {
public:
	typedef TObjectType ObjectType;
	
	SmartPointer () 
    { m_Pointer = 0; }
	
	SmartPointer (const SmartPointer<ObjectType> &p):
    m_Pointer(p.m_Pointer)
    { this->Register(); }
	
	SmartPointer (ObjectType *p):
    m_Pointer(p)
    { this->Register(); }                             
	
	SmartPointer (const WeakPointer<ObjectType> &p)
    {
		m_Pointer = p.GetPointer();
		this->Register(); 
    }
	
	~SmartPointer ()
    {
		this->UnRegister();
		m_Pointer = 0;  
    }
	
	ObjectType *operator -> () const
    { return m_Pointer; }
	
	operator ObjectType * () const 
    { return m_Pointer; }
	
	template <typename R>
		bool operator == (R r) const
    { return (m_Pointer == (ObjectType*)r); }
	
	template <typename R>
		bool operator != (R r) const
    { return (m_Pointer != (ObjectType*)r); }
    
	ObjectType *GetPointer () const 
    { return m_Pointer; }
	
	bool operator < (const SmartPointer &r) const
    { return (void*)m_Pointer < (void*) r.m_Pointer; }
	
	bool operator > (const SmartPointer &r) const
    { return (void*)m_Pointer > (void*) r.m_Pointer; }
	
	bool operator <= (const SmartPointer &r) const
    { return (void*)m_Pointer <= (void*) r.m_Pointer; }
	
	bool operator >= (const SmartPointer &r) const
    { return (void*)m_Pointer >= (void*) r.m_Pointer; }
	
	SmartPointer &operator = (const SmartPointer &r)
    { return this->operator = (r.GetPointer()); }
	
	SmartPointer &operator = (const WeakPointer<ObjectType> &r)
    { return this->operator = (r.GetPointer()); }
	
	SmartPointer &operator = (ObjectType *r)
    {                                                              
		if (m_Pointer != r)
		{
			ObjectType* tmp = m_Pointer;
			m_Pointer = r;
			this->Register();
			if ( tmp ) { tmp->UnRegister(); }
		}
		return *this;
    }
	
private:
	ObjectType* m_Pointer;
	
	void Register()
    { 
		if(m_Pointer) { m_Pointer->Register(); }
    }
	
	void UnRegister()
    {
		if(m_Pointer) { m_Pointer->UnRegister(); }
    }
};  


#endif
