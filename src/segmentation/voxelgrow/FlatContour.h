/*
# $Id: FlatContour.h,v 1.1 2003/05/02 22:21:59 sebasfiorent Exp $
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


#include <vtkStructuredPointsToPolyDataFilter.h>

#if !defined(_FLATCONTOUR_H_)
#define _FLATCONTOUR_H_


class FlatContour  : public vtkStructuredPointsToPolyDataFilter 
{
public:
	FlatContour();

	void setOutsideValue(float insideValue);
	float getOutsideValue();

	void setCenteredValues();
	void resetCenteredValues();

protected:
	float outsideValue;
	bool centeredValues;

	virtual void Execute();
	void inline loadarr(int pt[3],float x, float y, float z);
	void inline loadarr(float pt[3],float x, float y, float z);
};

#endif // !defined(_FLATCONTOUR_H_)
