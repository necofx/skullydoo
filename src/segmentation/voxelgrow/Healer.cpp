/*
# $Id: Healer.cpp,v 1.2 2003/05/06 00:12:14 sebasfiorent Exp $
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


#include "Healer.h"
#include "VoxelGrow.h"
#include <vtkPointData.h>

Healer::Healer():vtkSimpleImageToImageFilter(){
	casesCount=0;
}

Healer::~Healer(){
}

int Healer::getCasesCount(){
	return casesCount;
}

inline bool inBounds(int pos[],int bounds[]){
	return (pos[0]>=0 && pos[0]<bounds[0] && pos[1]>=0 && pos[1]<bounds[1] && pos[2]>=0 && pos[2]<bounds[2]);
}

inline bool Healer::isIn(int *pos,vtkImageData *output){
	int pointId;
	if (pos[0]<0 || pos[1]<0 || pos[2]<0 ||
		pos[0]>=tam[0] || pos[1]>=tam[1] || pos[2]>=tam[2]) {
		return false;
	}
	pointId = output->ComputePointId(pos);
	float value;
	value = scalars->GetComponent(pointId,0);
	return (value!=VoxelGrow::VALUE_REGION_OUT);
}

inline bool Healer::isPatologicalCase(int x,int y,int z,int caso,int &outId,vtkImageData *output){
	int vec[3];
	int vec1[3];
	int vec2[3];
	int vec3[3];
	int vec4[3];
	int bounds[3];
	output->GetDimensions(bounds);
	switch (caso){
	case 1:
    	vec[0] = x+1; vec[1] = y+1; vec[2] = z+1;
					
		vec1[0] = x+1; vec1[1] = y; vec1[2] = z;
		vec2[0] = x+1; vec2[1] = y; vec2[2] = z+1;
		vec3[0] = x; vec3[1] = y+1; vec3[2] = z;
		vec4[0] = x; vec4[1] = y+1; vec4[2] = z+1;
		break;
	case 2:
		vec[0] = x+1; vec[1] = y-1; vec[2] = z+1;
	
		vec1[0] = x+1; vec1[1] = y; vec1[2] = z;
		vec2[0] = x+1; vec2[1] = y; vec2[2] = z+1;
		vec3[0] = x; vec3[1] = y-1; vec3[2] = z;
		vec4[0] = x; vec4[1] = y-1; vec4[2] = z+1;
		break;
	case 3:
		vec[0] = x+1; vec[1] = y-1; vec[2] = z-1;
		
		vec1[0] = x+1; vec1[1] = y; vec1[2] = z;
		vec2[0] = x+1; vec2[1] = y; vec2[2] = z-1;
		vec3[0] = x; vec3[1] = y-1; vec3[2] = z;
		vec4[0] = x; vec4[1] = y-1; vec4[2] = z-1;
		break;
	case 4:
		vec[0] = x+1;vec[1] = y+1;vec[2] = z-1;
		
		vec1[0] = x+1; vec1[1] = y; vec1[2] = z;
		vec2[0] = x+1; vec2[1] = y; vec2[2] = z-1;
		vec3[0] = x; vec3[1] = y+1; vec3[2] = z;
		vec4[0] = x; vec4[1] = y+1; vec4[2] = z-1;
		break;
	case 5:
		vec[0] = x+1;vec[1] = y+1;vec[2] = z;
		
		vec1[0] = x+1; vec1[1] = y; vec1[2] = z;
		vec2[0] = x; vec2[1] = y+1; vec2[2] = z;
		break;
	case 6:
		vec[0] = x+1;vec[1] = y-1;vec[2] = z;
		
		vec1[0] = x+1; vec1[1] = y; vec1[2] = z;
		vec2[0] = x; vec2[1] = y-1; vec2[2] = z;
		break;
	case 7:
		vec[0] = x+1;vec[1] = y;vec[2] = z-1;
		
		vec1[0] = x+1; vec1[1] = y; vec1[2] = z;
		vec2[0] = x; vec2[1] = y; vec2[2] = z-1;
		break;
	case 8:
		vec[0] = x+1;vec[1] = y;vec[2] = z+1;
		
		vec1[0] = x+1; vec1[1] = y; vec1[2] = z;
		vec2[0] = x; vec2[1] = y; vec2[2] = z+1;
		break;
	case 9:
		vec[0] = x;vec[1] = y+1;vec[2] = z+1;
		
		vec1[0] = x; vec1[1] = y+1; vec1[2] = z;
		vec2[0] = x; vec2[1] = y; vec2[2] = z+1;
		break;
	case 10:
		vec[0] = x;vec[1] = y+1;vec[2] = z-1;
		
		vec1[0] = x; vec1[1] = y+1; vec1[2] = z;
		vec2[0] = x; vec2[1] = y; vec2[2] = z-1;
	}
	if (inBounds(vec1,bounds)) {
		outId=output->ComputePointId(vec1);
		//cout << vec1[0] <<","<<vec1[1] <<","<<vec1[2] <<"->"<< x <<","<<y <<","<<z <<"\n";
	}else if (inBounds(vec2,bounds)) {
		outId=output->ComputePointId(vec2);
		//cout << vec2[0] <<","<<vec2[1] <<","<<vec2[2] <<"->"<< x <<","<<y <<","<<z <<"\n";
	}else{
		int pos[3];
		pos[0]=x;
		pos[1]=y;
		pos[2]=z;
		outId=output->ComputePointId(pos);
	}
	if (isIn(vec,output)){
		if (1<=caso && caso<=4)
			return (!isIn(vec1,output) && !isIn(vec2,output) && !isIn(vec3,output) && !isIn(vec4,output) );
		else
			return (!isIn(vec1,output) && !isIn(vec2,output));
	}else{
		return false;
	}
}

void Healer::SimpleExecute(vtkImageData *input, vtkImageData *output){
	input->GetDimensions(tam);
	output->DeepCopy(input);
	scalars=(vtkFloatArray *)(output->GetPointData()->GetScalars());
	long total=tam[0]*tam[1]*tam[2];
	long step=total/20;
	long act=0;
	for (int z=0;z<tam[2];z++){
		for (int y=0;y<tam[1];y++){
			for (int x=0;x<tam[0];x++){
				act++;
				if (act%step==0) {
					UpdateProgress((float)act/(float)total);
				}
				int pos[3];
				pos[0]=x;
				pos[1]=y;
				pos[2]=z;
				int pointId = output->ComputePointId(pos);
				int outId;
				for (int c=1;c<=10;c++){
					if (isIn(pos,output) && isPatologicalCase(x,y,z,c,outId,output)){
						if (pointId%3==0) {
							scalars->SetComponent(pointId,0,VoxelGrow::VALUE_REGION_OUT);
							casesCount++;
						}else{
							float orig=scalars->GetComponent(pointId,0);
							scalars->SetComponent(outId,0,orig);
							casesCount++;
						}
					}
				}
			}
		}
		//cout << casesCount << "\n";
	}

}



