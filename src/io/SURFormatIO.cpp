/*
# $Id: SURFormatIO.cpp,v 1.3 2003/05/23 19:18:59 sebasfiorent Exp $
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

#include "SURFormatIO.h"
#include "gui/ProgressWindowGUI.h"
#include <vtkCell.h>
#include <vtkCellArray.h>
#include <stdlib.h>

bool SURFormatIO::write(std::string filename,vtkPolyData* polydata){
	FILE* output=fopen(filename.c_str(),"w");
	if (!output) return false;
	STRVector eg;
	STRVector ig;
	STRVector cg;
	STRVector fv;
	float bounds[6];
	fv.push_back("*FRONT_VOLUME");
	std::string temp;
	char holder[1024];
	int i;
	polydata->GetBounds(bounds);
	sprintf(holder,"%6.10f %6.10f %6.10f %6.10f %6.10f %6.10f",bounds[0],bounds[1],bounds[2],bounds[3],bounds[4],bounds[5]);
	fv.push_back(holder);
	// 
	ProgressWindowGUI::Instance()->doStartEvent(_("Saving .SUR file"));
	//Elementos (triangulos)
	eg.push_back("*ELEMENT GROUPS");
	eg.push_back(" 1");
	sprintf(holder,"%d",polydata->GetNumberOfCells());
	temp = std::string("1 ") + std::string(holder)+" Tri3";
	eg.push_back(temp);
	//Incidencia (puntos de cada triangulo)
	ig.push_back("*INCIDENCE");
	int nofcells=polydata->GetNumberOfCells();
	for (i=0;i<nofcells;i++){
		if (i%100==0){
			ProgressWindowGUI::Instance()->doProgressEvent(((float)i/(float)nofcells)*0.33);
		}
		vtkCell* cell=polydata->GetCell(i);
		int t = cell->GetCellType();
		//Solo escribo triangulos
		if (t == VTK_TRIANGLE){
			temp="";
			for (int j=0;j<cell->GetNumberOfPoints();j++){
				sprintf(holder,"%d",cell->GetPointId(j)+1);
				temp = temp + std::string(holder)+" ";
			}
			ig.push_back(temp);
		}
	}
	//Coordenadas
	cg.push_back("*COORDINATES");
	sprintf(holder,"%d",polydata->GetNumberOfPoints());
	cg.push_back(holder);
	int nop=polydata->GetNumberOfPoints();
	for (i=0;i<nop;i++){
		if (i%100==0){
			ProgressWindowGUI::Instance()->doProgressEvent(((float)i/(float)nop)*0.33+0.33);
		}
		float point[3];
	    polydata->GetPoint(i,point);
		sprintf(holder,"%d %6.10f %6.10f %6.10f",i+1,point[0],point[1],point[2]);
	    cg.push_back(holder);
	}

	STRVector::iterator e;
	int nol=eg.size()+ig.size()+cg.size()+fv.size();
	int lnum=0;
	
	for (e=eg.begin();e!=eg.end();e++){
		lnum++;
		if (lnum%100==0){
			ProgressWindowGUI::Instance()->doProgressEvent(((float)lnum/(float)nol)*0.33+0.66);
		}
		fputs((*e).c_str(),output);
	}
	
	for (e=ig.begin();e!=ig.end();e++){
		lnum++;
		if (lnum%100==0){
			ProgressWindowGUI::Instance()->doProgressEvent(((float)lnum/(float)nol)*0.33+0.66);
		}
		fputs((*e).c_str(),output);
	}
	
	for (e=cg.begin();e!=cg.end();e++){
		lnum++;
		if (lnum%100==0){
			ProgressWindowGUI::Instance()->doProgressEvent(((float)lnum/(float)nol)*0.33+0.66);
		}
		fputs((*e).c_str(),output);
	}
	
	for (e=fv.begin();e!=fv.end();e++){
		lnum++;
		if (lnum%100==0){
			ProgressWindowGUI::Instance()->doProgressEvent(((float)lnum/(float)nol)*0.33+0.66);
		}
		fputs((*e).c_str(),output);
	}
	ProgressWindowGUI::Instance()->doEndEvent();
	bool ok=!(ferror(output));
	fclose(output);
	return ok;
}

bool SURFormatIO::read(std::string filename,vtkPolyData* output){
	FILE* input=fopen(filename.c_str(),"r");
	if (!input) return false;
	
	STRVector eg;
	STRVector ig;
	STRVector cg;
	STRVector fv;
	std::string linea;
	char* clinea=new char[255];
	int grupo=-1;
	while(!feof(input)){
		fgets(clinea,255,input);
		linea=clinea;
		if (linea.find("*ELEMENT GROUPS")!=std::string::npos) grupo=1;
		else
		if (linea.find("*INCIDENCE")!=std::string::npos) grupo=2;
		else
		if (linea.find("*COORDINATES")!=std::string::npos) grupo=3;
		else
		if (linea.find("*FRONT_VOLUME")!=std::string::npos) grupo=4;
		else
			if (grupo!=-1){
				switch(grupo){
				case 1:eg.push_back(linea);
					break;
				case 2:ig.push_back(linea);
					break;
				case 3:cg.push_back(linea);
					break;
				default:fv.push_back(linea);
					break;
				}
			}
	}
	fclose(input);
	delete(clinea);
	vtkCellArray* polys=vtkCellArray::New();
	vtkPoints* points=vtkPoints::New();
	output->SetPoints(points);
	points->Delete();
	output->SetPolys(polys);
	polys->Delete();
	points->SetDataTypeToFloat();
	STRVector::iterator e;
	for (e=cg.begin()+1;e!=cg.end();e++){
		linea=*e;
		//separo en componentes
		int pos = linea.find(" ");
		std::string pid=linea.substr(0,pos);
		int ipid = atoi(pid.c_str());
		linea=linea.substr(pos+1);
		pos=linea.find(" ");
		std::string x = linea.substr(0,pos);
		float fcoord[3];
		fcoord[0]=atof(x.c_str());
		linea=linea.substr(pos+1);
		pos = linea.find(" ");
		std::string y = linea.substr(0,pos);
		fcoord[1]=atof(y.c_str());
		linea=linea.substr(pos+1);
		pos = linea.find(" ");
		std::string z = linea;
		fcoord[2]=atof(z.c_str());
		points->InsertPoint(ipid-1,fcoord);
	}
	//Leo todas las celdas (de incidence)
	for (e=ig.begin();e!=ig.end();e++){
		linea=*e;
	    //separo en componentes
	    int pos = linea.find(" ");
		std::string c = linea.substr(0,pos);
	    int pts[3];
	    pts[0] = atoi(c.c_str())-1;
	    linea=linea.substr(pos+1);
	    pos=linea.find(" ");
	    c = linea.substr(0,pos);
	    pts[1] = atoi(c.c_str())-1;
	    linea=linea.substr(pos+1);
		pts[2]= atoi(linea.c_str())-1;
		polys->InsertNextCell(3,pts);
	}
	return true;
}
