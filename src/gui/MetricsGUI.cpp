/*
# $Id: MetricsGUI.cpp,v 1.1 2003/05/02 22:21:53 sebasfiorent Exp $
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

#include "MetricsGUI.h"
#include <vtkPolyData.h>
#include <vtkTriangle.h>
#include <vtkMath.h>
#include <math.h>

MetricsGUI::MetricsGUI():MetricsGUIBase(){
}

void MetricsGUI::show(SurfaceModel::Pointer surface){
	//cte de un equilatero. area/perimetro
	float p1[3]={0,0,0};
	float p2[3]={3*0.866025403,0.5*3,0};
	float p3[3]={3*0.866025403,-0.5*3,0};
/*	float tarea=vtkTriangle::TriangleArea(p1,p2,p3);
	tp=tp+sqrt(vtkMath::Distance2BetweenPoints(p1,p2));
	tp=tp+sqrt(vtkMath::Distance2BetweenPoints(p2,p3));
	tp=tp+sqrt(vtkMath::Distance2BetweenPoints(p3,p1));

	float ctetriangequilataperim=(tarea/tp)/1;
	*/
	float ctetriang=1.0;
	char holder[1024];
	//sprintf(holder,"%4.5f",ctetriang);
	//icteequilatero->value(holder);
	//Recorrer todos los triangulos
	float vaccum=0;
	float vmin=1;
	int numprocessed=0;
	vtkPolyData* data=surface->getFilteredPolyData();
	data->BuildCells();
	for (int i=0;i<data->GetNumberOfCells();i++){
		vtkTriangle* t=vtkTriangle::SafeDownCast(data->GetCell(i));
		if (t){
			t->GetPoints()->GetPoint(0,p1);
			t->GetPoints()->GetPoint(1,p2);
			t->GetPoints()->GetPoint(2,p3);
			float center[3];
			vtkTriangle::TriangleCenter(p1,p2,p3,center);
			float d1,d2,d3;
			d1=sqrt(vtkMath::Distance2BetweenPoints(center,p1));
			d2=sqrt(vtkMath::Distance2BetweenPoints(center,p2));
			d3=sqrt(vtkMath::Distance2BetweenPoints(center,p3));
			float rinscript=d1;
			if (d2<rinscript) rinscript=d2;
			if (d3<rinscript) rinscript=d3;
			float rcircunstcript=d1;
			if (d2>rcircunstcript) rcircunstcript=d2;
			if (d3>rcircunstcript) rcircunstcript=d3;
			float factor=0;
			if (rcircunstcript>0){
				factor=rinscript/rcircunstcript;
				if (factor<vmin) vmin=factor;
			}
			numprocessed++;
			vaccum=vaccum+factor;
			// 
			//
			
			
		}
	}
	sprintf(holder,"%4.5f",(float)vaccum/(float)numprocessed);
	ipromrazonradiotriangulos->value(holder);
	sprintf(holder,"%4.5f",vmin);
	iminimorazonradiotriangulos->value(holder);
	sprintf(holder,"%d",numprocessed);
	itriangulos->value(holder);
	sprintf(holder,"%d",data->GetNumberOfPoints());
    ipuntos->value(holder);
	window->show();
	while(window->shown()){
		Fl::check();
	}
}
