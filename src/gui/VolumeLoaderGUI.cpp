/*
# $Id: VolumeLoaderGUI.cpp,v 1.2 2003/05/06 00:12:14 sebasfiorent Exp $
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

#include "VolumeLoaderGUI.h"

#include <FL/fl_ask.H>
#include <FL/filename.H>
#include "gui/ProgressWindowGUI.h"
#include "common/Types.h"
#include <vtkPointData.h>
#include <vtkImageCast.h>
#include <vtkImageSource.h>
#include <vtkImageReader.h>
#include <vtkVolume16Reader.h>
#include <vtkStructuredPoints.h>
#include <vtkBMPReader.h>
#include <vtkTIFFReader.h>
#include <vtkImageReader.h>
#include <vtkImageReader2.h>
#include <vtkPNMReader.h>
#include <vtkJPEGReader.h>
#include <vtkPNGReader.h>
#include <vtkCharArray.h>
#include <vtkUnsignedCharArray.h>
#include <vtkShortArray.h>
#include <vtkUnsignedShortArray.h>
#include <vtkIntArray.h>
#include <vtkUnsignedIntArray.h>
#include <vtkLongArray.h>
#include <vtkUnsignedLongArray.h>
#include <vtkFloatArray.h>
#include <vtkDoubleArray.h>
#include <vtkGESignaReader.h>
#include <algorithm>

typedef struct {
	int tx;
	int ty;
	int tz;
} Dim;

static char sampleLabel[1024];

VolumeLoaderGUI::VolumeLoaderGUI(std::string filename):VolumeLoaderGUIBase(){
	this->filename=filename;
	FILE* fp=fopen(filename.c_str(),"rb");
	fseek(fp,0,SEEK_END);
	filesize=ftell(fp);
	fclose(fp);

	std::string filenameExt;
	std::string filenamePref=filename;
	int pos;
	pos=filename.find_last_of(".");
	if (pos!=std::string::npos){
		filenameExt=filename.substr(pos+1,255);
		filenamePref=filename.substr(0,pos);
	}
	std::transform(filenameExt.begin(),filenameExt.end(),filenameExt.begin(),tolower);
	std::string mask="%s%d."+filenameExt;
	fnamePrefix->value(filenamePref.c_str());
	editMask->value(mask.c_str());
	sliceNum->value("1");
	sliceSep->value("1");
	sliceStart->value("1");
	updateSample();

	if (filenameExt=="bmp") comboFileFormats->value(0);
	else
	if (filenameExt=="jpeg" || filenameExt=="jpg") comboFileFormats->value(1);
	else
	if (filenameExt=="tiff" || filenameExt=="tif") comboFileFormats->value(2);
	else
	if (filenameExt=="png") comboFileFormats->value(3);
	else
	if (filenameExt=="ppm" || filenameExt=="pnm") comboFileFormats->value(4);
	else
		comboFileFormats->value(5);
	fileFormatChanged();
}

VolumeLoaderGUI::~VolumeLoaderGUI(){
	clearGuessedDimensions();
}

void VolumeLoaderGUI::clearGuessedDimensions(){
	for (int i=1;i<guessedDimensions->size();i++){
		Dim* d=(Dim*)guessedDimensions->data(i);
		delete d;
	}
	guessedDimensions->clear();
}

bool VolumeLoaderGUI::isCorrectDimension(int gx,int gy,int gz){
	int sizemult;
	if (rb8bit->value()) sizemult=1;
	else
	if (rb12bit->value()) sizemult=2;
	else
	if (rb16bit->value()) sizemult=2;
	else
	if (rb24bit->value()) sizemult=3;
	unsigned long size=gx*gy*gz*sizemult;
	return (size==filesize);
}

void VolumeLoaderGUI::updateSample(){
	const char* mask=editMask->value();
	const char* prefix=fnamePrefix->value();
	int start=atoi(sliceStart->value());
	sprintf(sampleLabel,mask,prefix,start);
	lblsample->label(sampleLabel);
	lblsample->redraw();
}

void VolumeLoaderGUI::guessDimensions(){
	const char* result;
	result=fl_input("Indice el valor maximo de alguna de las dimensiones","256");
	if (!result) return;
	int maxval=atoi(result);
	if (maxval<=0) {
		fl_alert("El valor ingresado es inválido. Debe se un número");
		return;
	}
	clearGuessedDimensions();
	unsigned long total=maxval*maxval*maxval;
	unsigned long actual=0;
	ProgressWindowGUI::Instance()->doStartEvent("Adivinando...");
	for (int gx=1;gx<maxval;gx++){
		for (int gy=1;gy<maxval;gy++){
			for (int gz=1;gz<maxval;gz++){
				actual++;
				if (actual%500==0) ProgressWindowGUI::Instance()->doProgressEvent((float)actual/(float)total);
				if (isCorrectDimension(gx,gy,gz)){
					Dim* d=new Dim;
					d->tx=gx;
					d->ty=gy;
					d->tz=gz;
					char hold[255];
					sprintf(hold,"%5d %5d %5d",gx,gy,gz);
					guessedDimensions->add(hold,d);
				}
			}
		}
	}
	ProgressWindowGUI::Instance()->doEndEvent();
	if (guessedDimensions->size()>0) guessedDimensions->activate();
	else
		guessedDimensions->deactivate();
}

void VolumeLoaderGUI::assignGuessed(){
	if (!guessedDimensions->value()) return;
	Dim* d=(Dim*)guessedDimensions->data(guessedDimensions->value());
	if (d!=0){
		char holder[255];
		sprintf(holder,"%d",d->tx);
		dimX->value(holder);
		sprintf(holder,"%d",d->ty);
		dimY->value(holder);
		sprintf(holder,"%d",d->tz);
		dimZ->value(holder);
	}
}

void VolumeLoaderGUI::invalidateGuessedDims(){
	clearGuessedDimensions();
	guessedDimensions->deactivate();
}

void VolumeLoaderGUI::fileFormatChanged(){
	int v=comboFileFormats->value();
	if (v==0 || v==1 || v==2 || v==3 || v==4){
		grRawFileOptions->deactivate();
	}
	else
		grRawFileOptions->activate();
}

bool VolumeLoaderGUI::execute(){
	window->show();
	while(window->shown()){
		Fl::check();
	}
	return (bool)window->user_data();
}

ImageModel::Pointer VolumeLoaderGUI::readVolume(){
	ImageModel::Pointer output=ImageModel::New();
	vtkProcessObject* po=0;
	vtkStructuredPoints* temporal=vtkStructuredPoints::New();
	FILE_TYPES f=(FILE_TYPES)comboFileFormats->value();
	float spacing[3]={atof(sepX->value()),atof(sepY->value()),atof(sepZ->value())};
	if (f==FORMAT_VOL16){
		vtkVolume16Reader* imr=vtkVolume16Reader::New();
		imr->SetDataDimensions(atoi(dimX->value()),atoi(dimY->value()));
		if (rbBE->value()) imr->SetDataByteOrderToBigEndian();
		else
			imr->SetDataByteOrderToLittleEndian();
		imr->SetFilePattern(editMask->value());
		imr->SetFilePrefix(fnamePrefix->value());
		imr->SetImageRange(atoi(sliceStart->value()),atoi(sliceNum->value()));
		imr->SetDataSpacing(spacing);
		ProgressWindowGUI::Instance()->Observe(imr,"Cargando volumen",filename);
		imr->SetOutput(temporal);
		po=imr;
  	}
	if (f==FORMAT_HUMAN_PROJECT){
		vtkGESignaReader* imr=vtkGESignaReader::New();
		/*if (rbBE->value()) imr->SetDataByteOrderToBigEndian();
		else
			imr->SetDataByteOrderToLittleEndian();

		imr->SetDataOrigin(0,0,0);
		imr->SetNumberOfScalarComponents(1);
		*/
		imr->SetDataSpacing(spacing);
		if (rbSliceFile->value()){
			imr->SetFileDimensionality(2);
			int s=atoi(sliceStart->value());
			int e=atoi(sliceStart->value())+atoi(sliceNum->value());
			imr->SetDataExtent(0,0,0,0,s,e);
			imr->SetFilePattern(editMask->value());
			imr->SetFilePrefix(fnamePrefix->value());
		}
		else{
/*			imr->SetFileDimensionality(3);
			imr->SetFileName(filename.c_str());
			imr->SetDataExtent(0,atoi(dimX->value())-1,0,atoi(dimY->value())-1,0,atoi(dimZ->value())-1);
			*/
		}
		ProgressWindowGUI::Instance()->Observe(imr,"Cargando volumen",filename);
		imr->SetOutput(temporal);
		po=imr;
	}
	if (f==FORMAT_TIFF){
		vtkTIFFReader* imr;
		if (rbSliceFile->value()){
			imr->SetNumberOfScalarComponents(1);
			imr->SetDataExtent(0,0,0,0,0,atoi(sliceNum->value())-1);
			imr->SetFilePattern(editMask->value());
			imr->SetFilePrefix(fnamePrefix->value());
			imr->SetFileNameSliceOffset(atoi(sliceStart->value()));
			imr->SetFileNameSliceSpacing(atoi(sliceSep->value()));
		}
		else{
			imr->SetFileName(filename.c_str());
		}
		imr->SetDataSpacing(spacing);
		ProgressWindowGUI::Instance()->Observe(imr,"Cargando volumen",filename);
		imr->SetOutput(temporal);
		po=imr;
	}
	if (f==FORMAT_BMP || f==FORMAT_PPM || f==FORMAT_RAW){
		vtkImageReader* imr;
		if (f!=FORMAT_RAW){
			if (f==FORMAT_BMP) imr=vtkBMPReader::New();
			if (f==FORMAT_PPM) imr=vtkPNMReader::New();
			if (rbSliceFile->value()){
				imr->SetNumberOfScalarComponents(1);
				imr->SetDataExtent(0,0,0,0,0,atoi(sliceNum->value())-1);
				imr->SetFilePattern(editMask->value());
				imr->SetFilePrefix(fnamePrefix->value());
				imr->SetFileNameSliceOffset(atoi(sliceStart->value()));
				imr->SetFileNameSliceSpacing(atoi(sliceSep->value()));
			}
			else{
				imr->SetFileName(filename.c_str());
			}
		}
		else{
			imr=vtkImageReader::New();
			if (rb8bit->value()) imr->SetDataScalarType(VTK_UNSIGNED_CHAR);
			if (rb12bit->value()){
				imr->SetDataScalarType(VTK_UNSIGNED_SHORT);
				imr->SetDataMask(0x0fff);
			}
			if (rb16bit->value()) imr->SetDataScalarType(VTK_UNSIGNED_SHORT);
			if (rb24bit->value()) imr->SetDataScalarType(VTK_UNSIGNED_INT);
			if (rbBE->value()) imr->SetDataByteOrderToBigEndian();
			else
				imr->SetDataByteOrderToLittleEndian();
			imr->SetNumberOfScalarComponents(1);
			if (rbSliceFile->value()){
				imr->SetDataExtent(0,atoi(dimX->value())-1,0,atoi(dimY->value())-1,0,atoi(sliceNum->value())-1);
				imr->SetFilePattern(editMask->value());
				imr->SetFilePrefix(fnamePrefix->value());
				imr->SetFileNameSliceOffset(atoi(sliceStart->value()));
				imr->SetFileNameSliceSpacing(atoi(sliceSep->value()));
			}
			else{
				imr->SetFileDimensionality(3);
				imr->SetFileName(filename.c_str());
				imr->SetDataExtent(0,atoi(dimX->value())-1,0,atoi(dimY->value())-1,0,atoi(dimZ->value())-1);
			}
		}
		imr->SetDataSpacing(spacing);
		ProgressWindowGUI::Instance()->Observe(imr,"Cargando volumen",filename);
		imr->SetOutput(temporal);
		po=imr;
	}
	if (po){
		output->setLabel(fl_filename_name(filename.c_str()));
		vtkImageData* uncasted;
		int dims[3];
		temporal->Update();
		temporal->GetDimensions(dims);
		if (dims[2]==1){
			uncasted=vtkStructuredPoints::New();
			transform2DVolTo3DVol(temporal,uncasted);
		}
		else{
			uncasted=temporal;
		}
		if (cbConvertToFloat->value()){
			vtkImageCast* ic=vtkImageCast::New();
			ProgressWindowGUI::Instance()->Observe(ic,"Haciendo casting al tipo del voxel",filename);
			ic->SetInput(uncasted);
			ic->SetOutputScalarType(VTK_VoxelType);
			ic->SetOutput(output->getInputVtkVolume());
			ic->Update();
			uncasted->ReleaseData();
			ic->Delete();
		}
		else{
			output->getInputVtkVolume()->DeepCopy(uncasted);
		}
		po->Delete();
	}
	temporal->Delete();
	return output;
}

void VolumeLoaderGUI::transform2DVolTo3DVol(vtkImageData* input,vtkImageData* output){
	int idims[3];
	int odims[3];
	float spacing[3];
	input->GetDimensions(idims);
	input->GetSpacing(spacing);
	odims[0]=idims[0];
	odims[1]=idims[0];
	odims[2]=idims[1]/idims[0];
	output->SetDimensions(odims);
	output->SetSpacing(spacing);
	output->SetOrigin(0,0,0);
	vtkDataArray* da;
	switch(input->GetScalarType()) {
	case VTK_BIT:
		output->SetScalarTypeToUnsignedChar();
		da=vtkUnsignedCharArray::New();
		break;
	case VTK_UNSIGNED_CHAR:
		output->SetScalarTypeToUnsignedChar();
		da=vtkUnsignedCharArray::New();
		break;
	case VTK_CHAR:
		output->SetScalarTypeToChar();
		da=vtkCharArray::New();
		break;
	case VTK_UNSIGNED_INT:
		output->SetScalarTypeToUnsignedInt();
		da=vtkUnsignedIntArray::New();
		break;
	case VTK_UNSIGNED_SHORT:
		output->SetScalarTypeToUnsignedShort();
		da=vtkUnsignedShortArray::New();
		break;
	case VTK_SHORT:
		output->SetScalarTypeToShort();
		da=vtkShortArray::New();
		break;		
	case VTK_LONG:
		output->SetScalarTypeToLong();
		da=vtkLongArray::New();
		break;
	case VTK_UNSIGNED_LONG:
		output->SetScalarTypeToUnsignedLong();
		da=vtkUnsignedLongArray::New();
		break;
	case VTK_FLOAT:
		output->SetScalarTypeToFloat();
		da=vtkFloatArray::New();
		break;
	case VTK_DOUBLE:
		output->SetScalarTypeToDouble();
		da=vtkDoubleArray::New();
		break;
	}
	output->GetPointData()->SetScalars(da);
	da->Delete();
	unsigned long total=idims[0]*idims[1];
	unsigned long copied=0;
	unsigned long step=total/20;
	//Copio
	ProgressWindowGUI::Instance()->doStartEvent("Convirtiendo imagen 2D en volumen 3D");
	for (int y=0;y<idims[1];y++)
		for (int x=0;x<idims[0];x++){
			copied++;
			if (copied%step==0){
				float prog=(float)copied/(float)total;
				ProgressWindowGUI::Instance()->doProgressEvent(prog);
			}
			int z=y/idims[1];
			int ipos[]={x,y,0};
			int opos[]={x,y%idims[1],z};
			int ipid=input->ComputePointId(ipos);
			int opid=output->ComputePointId(opos);
			float d=input->GetPointData()->GetScalars()->GetComponent(ipid,0);
			da->InsertComponent(opid,0,d);
		}
	ProgressWindowGUI::Instance()->doEndEvent();
}
