/*
# $Id: GradientGrowth.cpp,v 1.3 2003/05/23 19:18:59 sebasfiorent Exp $
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

#include "GradientGrowth.h"

#include "common/ImageModel.h"
#include "gui/ProgressWindowGUI.h"
#include <queue>
#include <math.h>
#include <vtkMath.h>
#include <vtkPointData.h>
#include <vtkCellData.h>
#include <vtkPolygon.h>
#include <vtkTriangle.h>
#include <vtkPolyDataNormals.h>

GradientGrowth::GradientGrowth(){
	gradientLimit=-1;
	maxiterations=-1;
	meandistance=-1;
	igCalculator=vtkImageGradient::New();
}

GradientGrowth::~GradientGrowth(){
	igCalculator->Delete();
}

void GradientGrowth::setInputGradientImage(vtkImageData* image){
	igCalculator->SetInput(image);
	igCalculator->SetDimensionality(3);
	ProgressWindowGUI::Instance()->Observe(igCalculator,_("Calculating gradient"),"unknown");
	igCalculator->Update();
	gradientImage=igCalculator->GetOutput();
	float spacing[3];
	gradientImage->GetSpacing(spacing);
}

void GradientGrowth::setGradientLimit(float gradientLimit){
	if (this->gradientLimit!=gradientLimit){
		this->gradientLimit=gradientLimit;
		this->Modified();
	}
}

void GradientGrowth::setMaxIterations(int maxiterations){
	if (this->maxiterations!=maxiterations){
		this->maxiterations=maxiterations;
		this->Modified();
	}
}

void GradientGrowth::setMeanDistance(float meandistance){
	if (this->meandistance!=meandistance){
		this->meandistance=meandistance;
		this->Modified();
	}
}

void GradientGrowth::Execute(){
	typedef std::pair<int,float> QueuePairType;
	typedef std::queue<QueuePairType> PointQueueType;
	PointQueueType queue;
	PointQueueType nextqueue;
	float imgbounds[6];
	float imgspacing[3];
	gradientImage->GetBounds(imgbounds);
	gradientImage->GetSpacing(imgspacing);
	vtkPolyData* input=GetInput();
	int nop=input->GetNumberOfPoints();
	int progstep=nop/20;
	for (int pnum=0;pnum<nop;pnum++){
		float pointpos[3];
		input->GetPoint(pnum,pointpos);
		if (pointpos[0]>imgbounds[0] && pointpos[0]<imgbounds[1] &&
			pointpos[1]>imgbounds[2] && pointpos[1]<imgbounds[3] &&
			pointpos[2]>imgbounds[4] && pointpos[2]<imgbounds[5]){
			float gx=ImageModel::interpolateColor(pointpos,gradientImage,0);
			float gy=ImageModel::interpolateColor(pointpos,gradientImage,1);
			float gz=ImageModel::interpolateColor(pointpos,gradientImage,2);
			float gradientValueAtPoint=sqrt(gx*gx+gy*gy+gz*gz);
			float currentGradDiff=gradientLimit-gradientValueAtPoint;
			if (currentGradDiff>0){
				QueuePairType pair;
				pair.first=pnum;
				pair.second=currentGradDiff;
				queue.push(pair);
			}
		}
	}
	vtkPolyData* output=GetOutput();
	output->CopyStructure(input);
    output->GetPointData()->PassData(input->GetPointData());
    output->GetCellData()->PassData(input->GetCellData());
    output->SetFieldData(input->GetFieldData());
	output->BuildLinks();
	vtkPolyData* temporal=vtkPolyData::New();
	temporal->CopyStructure(output);
	temporal->GetPointData()->PassData(output->GetPointData());
	temporal->GetCellData()->PassData(output->GetCellData());
	temporal->SetFieldData(output->GetFieldData());
	vtkPolyDataNormals* normals=vtkPolyDataNormals::New();
	normals->SetInput(temporal);
	normals->SplittingOff();
	normals->ConsistencyOn();
	normals->ComputeCellNormalsOff();
	normals->ComputePointNormalsOn();
	normals->FlipNormalsOff();
	normals->NonManifoldTraversalOn();
	//Armo la cola inicial
	vtkIdList* celllist=vtkIdList::New();
	//Bucle principal.
	int iternum=1;
	normals->Update();
	while (iternum<=maxiterations && queue.size()>0){
		QueuePairType point;
		point=queue.front();
		queue.pop();
		// 
		float pointpos[3];
		temporal->GetPoint(point.first,pointpos);
		float normal[3];
		normals->GetOutput()->GetPointData()->GetTuple(point.first,normal);
		//Me desplazo
		float ctex=(gradientLimit-ImageModel::interpolateColor(pointpos,gradientImage,0))/gradientLimit;
		float ctey=(gradientLimit-ImageModel::interpolateColor(pointpos,gradientImage,1))/gradientLimit;
		float ctez=(gradientLimit-ImageModel::interpolateColor(pointpos,gradientImage,2))/gradientLimit;
		pointpos[0]=pointpos[0]+(normal[0]*imgspacing[0]*ctex*0.5);
		pointpos[1]=pointpos[1]+(normal[1]*imgspacing[1]*ctey*0.5);
		pointpos[2]=pointpos[2]+(normal[2]*imgspacing[2]*ctez*0.5);
		if (pointpos[0]>imgbounds[0] && pointpos[0]<imgbounds[1] &&
			pointpos[1]>imgbounds[2] && pointpos[1]<imgbounds[3] &&
			pointpos[2]>imgbounds[4] && pointpos[2]<imgbounds[5]){
			float gx=ImageModel::interpolateColor(pointpos,gradientImage,0);
			float gy=ImageModel::interpolateColor(pointpos,gradientImage,1);
			float gz=ImageModel::interpolateColor(pointpos,gradientImage,2);
			float gv=sqrt(gx*gx+gy*gy+gz*gz);
			float currentGradDiff=gradientLimit-gv;
			//Si mejoro la diferencia, calculo la nueva posicion, asigno y encolo
			if (currentGradDiff>=0 && currentGradDiff<=point.second){
				temporal->GetPoints()->SetPoint(point.first,pointpos);
				point.second=currentGradDiff;
				nextqueue.push(point);
			}
		}
		// 
		if (queue.size()==0){
			output->CopyStructure(temporal);
			output->GetPointData()->PassData(temporal->GetPointData());
			output->GetCellData()->PassData(temporal->GetCellData());
			output->SetFieldData(temporal->GetFieldData());
			queue=nextqueue;
			nextqueue=PointQueueType();
			normals->Update();
			iternum++;
		}
	}
	temporal->Delete();
	normals->Delete();
	celllist->Delete();
}
