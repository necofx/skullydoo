/*
# $Id: VoxelGrow.cpp,v 1.1 2003/05/02 22:21:59 sebasfiorent Exp $
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

#include "VoxelGrow.h"

#include "common/Types.h"
#include "gui/ProgressWindowGUI.h"

#include <vtkDataArray.h>
#include <vtkFloatArray.h>
#include <vtkStructuredPoints.h>

float VoxelGrow::VALUE_REGION_OUT=0;

VoxelGrow::VoxelGrow():vtkSimpleImageToImageFilter(){
	stretchedPoints=vtkBitArray::New();
}

VoxelGrow::~VoxelGrow(){
	stretchedPoints->Delete();
}

void VoxelGrow::setConfig(ConfigVector config){
	this->config=config;
}

VoxelGrow::ConfigVector VoxelGrow::getConfig(){
	return config;
}

void VoxelGrow::setRadius(int radius){
	this->radius=radius;
}

void VoxelGrow::setStretchRadius(int stretchradius){
	this->stretchradius=stretchradius;
}

void VoxelGrow::SimpleExecute(vtkImageData* input,vtkImageData* output){
	if (config.size()==0) return;

    int extent[6];
	input->GetExtent(extent);
	int dims[3];
	int neighborcases[6][3]={
		{0,0,1},
		{0,0,-1},
		{0,1,0},
		{0,-1,0},
		{1,0,0},
		{-1,0,0}
	};
	input->GetDimensions(dims);
	stretchedPoints->SetNumberOfTuples(dims[0]*dims[1]*dims[2]);
	float orig[3];
	input->GetOrigin(orig);
	float spac[3];
	input->GetSpacing(spac);

	output->SetDimensions(dims);
	output->SetOrigin(orig);
	output->SetSpacing(spac);

	vtkDataArray* odata=output->GetPointData()->GetScalars();
	odata->SetNumberOfTuples(dims[0]*dims[1]*dims[2]);
	odata->FillComponent(0,VALUE_REGION_OUT);

	int ipid;
	int opid;
	int offset=radius;
	int neighcount=(offset*2+1)*(offset*2+1);
	VoxelModel* sideneighbors=new VoxelModel[neighcount];

	VoxelProcessor::Pointer vprocessor;


	unsigned long totalpoints=dims[0]*dims[1]*dims[2];
	unsigned long visitedpoints=0;
	unsigned long step=(totalpoints/100);
	unsigned long maxmark=1;
	
	//Para cada par de la configuracion, hago todo
	for (ConfigVector::iterator cfg=config.begin();cfg!=config.end();cfg++){
		
		VoxelModel::Queue pq;
		VoxelModel::Queue nextpq;
		VoxelModel point;
		VoxelModel neigh;

		point=cfg->first;
		vprocessor=cfg->second;

		int ox;
		int oy;
		int oz;
		//Marco todo lo que esta adentro
		for (ox=point.x-offset;ox<=point.x+offset;ox++)
			for (oy=point.y-offset;oy<=point.y+offset;oy++)
				for (oz=point.z-offset;oz<=point.z+offset;oz++){
					if (ox>=extent[0] && ox<=extent[1] && 
						oy>=extent[2] && oy<=extent[3] &&
						oz>=extent[4] && oz<=extent[5]){
							int pos[3]={ox-extent[0],oy-extent[2],oz-extent[4]};
							opid=output->ComputePointId(pos);
							output->GetPointData()->SetComponent(opid,0,1);
					}
				}
		//Encolo la semilla
		pq.push(point);
		//Itero
		stretchedPoints->FillComponent(0,0);
		//
		vprocessor->setSeed(point);
		vprocessor->setInputImage(input);
		vprocessor->initialize();
		
		int sideneighaddedcount=0;
		int count=0;
		while (!pq.empty()) {
			if (visitedpoints%step==0)UpdateProgress(0.8*((float)visitedpoints/(float)totalpoints));
			visitedpoints++;
			point=pq.front();
			
			float pointval;
			{
				int ppos[]={point.x-extent[0],point.y-extent[2],point.z-extent[4]};
				opid=output->ComputePointId(ppos);
				pointval=output->GetPointData()->GetComponent(opid,0);
			}
			for (int casenum=0;casenum<6;casenum++){

				int neighsidenum;

				neigh.x=point.x+neighborcases[casenum][0];
				neigh.y=point.y+neighborcases[casenum][1];
				neigh.z=point.z+neighborcases[casenum][2];
				//Avance en X
				if (neighborcases[casenum][0]>0){
					neighsidenum=1;
					int nc=0;
					ox=neigh.x+offset;
					for (oy=-offset;oy<=offset;oy++)
						for (oz=-offset;oz<=offset;oz++){
							sideneighbors[nc].x=ox;
							sideneighbors[nc].y=neigh.y+oy;
							sideneighbors[nc].z=neigh.z+oz;
							nc++;
						}

				}
				else
				//Retroceso en X
				if (neighborcases[casenum][0]<0){
					neighsidenum=-1;
					int nc=0;
					ox=neigh.x-offset;
					for (oy=-offset;oy<=offset;oy++)
						for (oz=-offset;oz<=offset;oz++){
							sideneighbors[nc].x=ox;
							sideneighbors[nc].y=neigh.y-oy;
							sideneighbors[nc].z=neigh.z-oz;
							nc++;
						}
				}
				else
				//Avance en Y
				if (neighborcases[casenum][1]>0){
					neighsidenum=2;
					int nc=0;
					oy=neigh.y+offset;
					for (ox=-offset;ox<=offset;ox++)
						for (oz=-offset;oz<=offset;oz++){
							sideneighbors[nc].x=neigh.x+ox;
							sideneighbors[nc].y=oy;
							sideneighbors[nc].z=neigh.z+oz;
							nc++;
						}
				}
				else
				//Retroceso en Y
				if (neighborcases[casenum][1]<0){
					neighsidenum=-2;
					int nc=0;
					oy=neigh.y-offset;
					for (ox=-offset;ox<=offset;ox++)
						for (oz=-offset;oz<=offset;oz++){
							sideneighbors[nc].x=neigh.x-ox;
							sideneighbors[nc].y=oy;
							sideneighbors[nc].z=neigh.z-oz;
							nc++;
						}
				}
				else
				//Avance en Z
				if (neighborcases[casenum][2]>0){
					neighsidenum=3;
					int nc=0;
					oz=neigh.z+offset;
					for (ox=-offset;ox<=offset;ox++)
						for (oy=-offset;oy<=offset;oy++){
							sideneighbors[nc].x=neigh.x+ox;
							sideneighbors[nc].y=neigh.y+oy;
							sideneighbors[nc].z=oz;
							nc++;
						}
				}
				else
				//Retroceso en Z
				if (neighborcases[casenum][2]<0){
					neighsidenum=-3;
					int nc=0;
					oz=neigh.z-offset;
					for (ox=-offset;ox<=offset;ox++)
						for (oy=-offset;oy<=offset;oy++){
							sideneighbors[nc].x=neigh.x-ox;
							sideneighbors[nc].y=neigh.y-oy;
							sideneighbors[nc].z=oz;
							nc++;
						}
				}
				bool acceptside=true;
				bool unmarkedinside=false;
				for (int nc=0;nc<neighcount && acceptside;nc++){
					if (sideneighbors[nc].x>=extent[0] && sideneighbors[nc].x<=extent[1] && 
						sideneighbors[nc].y>=extent[2] && sideneighbors[nc].y<=extent[3] &&
						sideneighbors[nc].z>=extent[4] && sideneighbors[nc].z<=extent[5]){
						int pos[3]={sideneighbors[nc].x-extent[0],sideneighbors[nc].y-extent[2],sideneighbors[nc].z-extent[4]};
						opid=output->ComputePointId(pos);
						bool marked=(odata->GetComponent(opid,0)!=0);
						unmarkedinside=unmarkedinside || !marked;
						if (!marked) acceptside=(vprocessor->processNeighbor(point,sideneighbors[nc]));
					}
				}	
				acceptside=acceptside && unmarkedinside;
				if (acceptside){
					for (int nc=0;nc<neighcount;nc++){
						if (sideneighbors[nc].x>=extent[0] && sideneighbors[nc].x<=extent[1] && 
							sideneighbors[nc].y>=extent[2] && sideneighbors[nc].y<=extent[3] &&
							sideneighbors[nc].z>=extent[4] && sideneighbors[nc].z<=extent[5]){
							int pos[3]={sideneighbors[nc].x-extent[0],sideneighbors[nc].y-extent[2],sideneighbors[nc].z-extent[4]};
							opid=output->ComputePointId(pos);
							odata->SetComponent(opid,0,pointval+1.0);
							if (pointval+1>maxmark) maxmark=(unsigned long)pointval+1;
						}
					}
					pq.push(neigh);
				}
				else{
					if (stretchradius>0){
						for (int nc=0;nc<neighcount;nc++){
							switch(neighsidenum) {
							case 1:sideneighbors[nc].x--;
								break;
							case -1:sideneighbors[nc].x++;
								break;
							case 2:sideneighbors[nc].y--;
								break;
							case -2:sideneighbors[nc].y++;
								break;
							case 3:sideneighbors[nc].z--;
								break;
							case -3:sideneighbors[nc].z++;
							}
							if (sideneighbors[nc].x>=extent[0] && sideneighbors[nc].x<=extent[1] && 
								sideneighbors[nc].y>=extent[2] && sideneighbors[nc].y<=extent[3] &&
								sideneighbors[nc].z>=extent[4] && sideneighbors[nc].z<=extent[5]){
								int pos[3]={sideneighbors[nc].x-extent[0],sideneighbors[nc].y-extent[2],sideneighbors[nc].z-extent[4]};
								opid=output->ComputePointId(pos);
								if (stretchedPoints->GetValue(opid)==0){
									stretchedPoints->SetValue(opid,1);
									nextpq.push(sideneighbors[nc]);
									sideneighaddedcount++;
								}
								/*if (odata->GetComponent(opid,0)>EXTENT_VOXEL_VAL){
									odata->SetComponent(opid,0,EXTENT_VOXEL_VAL);
									nextpq.push(sideneighbors[nc]);
									sideneighaddedcount++;
								}
								*/
							}
						}
					}
				}
			}
			pq.pop();
		}
		//Ultima pasada...Tiro de los limites con radio 1
		visitedpoints=0;
		step=(sideneighaddedcount/20)+1;
		while (!nextpq.empty()) {
			visitedpoints++;
			if (visitedpoints%step==0)UpdateProgress(0.8+((float)visitedpoints/(float)sideneighaddedcount)*0.2);
			point=nextpq.front();
			int pos[3]={point.x-extent[0],point.y-extent[2],point.z-extent[4]};
			ipid=output->ComputePointId(pos);
			float pvalue=odata->GetComponent(ipid,0);
			for (int casenum=0;casenum<6;casenum++){
				neigh.x=point.x+neighborcases[casenum][0];
				neigh.y=point.y+neighborcases[casenum][1];
				neigh.z=point.z+neighborcases[casenum][2];
				if (point.x>=extent[0] && point.x<=extent[1] && 
					point.y>=extent[2] && point.y<=extent[3] &&
					point.z>=extent[4] && point.z<=extent[5] &&
					neigh.x>=extent[0] && neigh.x<=extent[1] && 
					neigh.y>=extent[2] && neigh.y<=extent[3] &&
					neigh.z>=extent[4] && neigh.z<=extent[5]){
					int npos[3]={neigh.x-extent[0],neigh.y-extent[2],neigh.z-extent[4]};
					opid=output->ComputePointId(npos);
					float nvalue=odata->GetComponent(opid,0);
					if (nvalue==0){	//Al menos el vecino esta por ser visitado
						//1. Verifico el valor de marca del punto actual
						//2. Si es < a maxmark lo pongo como maxmark
						if (pvalue<=maxmark) {
							pvalue=maxmark+1;
							odata->SetComponent(ipid,0,pvalue);
						}
						if ((pvalue-maxmark)<=stretchradius){
							if (vprocessor->processNeighbor(point,neigh)){
								odata->SetComponent(opid,0,pvalue+1);
								nextpq.push(neigh);
							}
						}
					}
				}
			}
			nextpq.pop();
		}
		vprocessor->finalize();
	}
	UpdateProgress(1.0);
	delete sideneighbors;
}
