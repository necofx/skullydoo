/*
# $Id: DesktopGUI.cpp,v 1.7 2004/09/01 11:48:47 nacholarrabide Exp $
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

#include "DesktopGUI.h"
#include "common/Config.h"
#include "io/SURFormatIO.h"

#include "gui/ProgressWindowGUI.h"
#include "gui/VolumeLoaderGUI.h"
#include "gui/TCLFilterPipelineConfigGUI.h"
#include "gui/MetricsGUI.h"
#include "tcl/TCL.h"
#include "tcl/TCLGlobals.h"
#include "tcl/TCLFilterPipeline.h"
#include "gui/TCLFilterConfigGUI.h"
#include "tcl/TCLFilterUtils.h"
#include "gui/AboutGUI.h"

#include "segmentation/activecontour/ActiveContourSegmentation.h"
#include "segmentation/fastmarching/FastMarchingSegmentation.h"
#include "segmentation/voxelgrow/VoxelGrowSegmentation.h"
#include "segmentation/voxelgrow/FlatContour.h"
#include "segmentation/voxelgrow/GradientProcessor.h"
#include "segmentation/voxelgrow/NeighborDeltaProcessor.h"
#include "segmentation/voxelgrow/SeedDeltaProcessor.h"

#include <FL/fl_ask.H>
#include <FL/fl_draw.H>
#include <FL/Enumerations.H>
#include <FL/Fl_Color_Chooser.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Shared_Image.H>
#include <FL/filename.H>

#include <vtkPolyDataMapper.h>
#include <vtkPolyDataNormals.h>
#include <vtkMarchingCubes.h>
#include <vtkDataSetWriter.h>
#include <vtkDataSetReader.h>
#include <vtkImageCast.h>
#include <vtkCellPicker.h>
#include <vtkBMPReader.h>
#include <vtkPNGReader.h>
#include <vtkPNGWriter.h>
#include <vtkWindowToImageFilter.h>
#include <vtkJPEGReader.h>
#include <vtkJPEGWriter.h>
#include <vtkStructuredPointsReader.h>
#include <vtkStructuredPointsWriter.h>
#include <vtkPolyDataReader.h>
#include <vtkPolyDataWriter.h>
#include <vtkWindowedSincPolyDataFilter.h>
#include <vtkPointData.h>
#include <vtkProperty.h>
#include <vtkTriangleFilter.h>
#include <vtkSTLWriter.h>
#include <vtkSTLReader.h>

#include <FL/x.H>
#ifdef _WIN32
#include "win32/resource.h"
#endif
#if !defined(WIN32) && !defined(__APPLE__)
#include "images/skull.xpm"
#endif


void imageFilterCallback(Fl_Widget* widget,void* data){
	TCLFilter::Pointer filter=(TCLFilter*)data;
	DesktopGUI::Instance()->applyImageToImageFilter(filter);
}

void surfaceFilterCallback(Fl_Widget* widget,void* data){
	TCLFilter::Pointer filter=(TCLFilter*)data;
	DesktopGUI::Instance()->applyPolyDataToPolyDataFilter(filter);
}


DesktopGUI* DesktopGUI::instance=0;

DesktopGUI* DesktopGUI::Instance(){
	if (!instance) instance=new DesktopGUI();
	return instance;
}

DesktopGUI::DesktopGUI():DesktopGUIBase(),vtkInteractorStyleSwitch(){
	fl_register_images();
#if (VTK_MAJOR_VERSION == 4 && VTK_MINOR_VERSION == 0)
	this->TrackballMode=VTKIS_TRACK;
#else
	this->SetCurrentStyleToTrackballActor();
#endif

	win3D=vtkRenderWindow::New();
	vtkRenderer* rr=vtkRenderer::New();
	win3D->AddRenderer(rr);
	rr->Delete();
	vtk3D->SetRenderWindow(win3D);
	vtkCellPicker* cp=vtkCellPicker::New();
#if (VTK_MAJOR_VERSION == 4 && VTK_MINOR_VERSION == 0)
	vtk3D->SetInteractorStyle(this);
#endif
	vtk3D->SetPicker(cp);
	vtk3D->Initialize();
	cp->Delete();
	renSurfaces=vtkRenderer::New();
	winSurfaces=vtkRenderWindow::New();
	winSurfaces->AddRenderer(renSurfaces);
	ProgressWindowGUI::Instance()->Observe(winSurfaces);
	vtkSurfaces->SetRenderWindow(winSurfaces);
#if (VTK_MAJOR_VERSION == 4 && VTK_MINOR_VERSION == 0)
	vtkSurfaces->SetInteractorStyle(this);
#endif
	vtkSurfaces->Initialize();
	// Filtros de imagen
	i2ifilters=TCLFilterUtils::hasOutputType("ImageData",TCLFilterUtils::hasInputType("ImageData",TCLFilterUtils::getAvailableFilters(Config::Instance()->getFiltersDirectory())));
	TCLFilter::Vector::iterator ifit;
	for (ifit=i2ifilters.begin();ifit!=i2ifilters.end();ifit++){
		TCLFilter::Pointer filter=(*ifit);
		std::string fname=filter->getName();
		if (fname.length()>40) fname=fname.substr(0,40);
		fname=std::string(_("&Filters/&3D Image/"))+fname;
		int idx=menu->add(fname.c_str(),0,(Fl_Callback*)imageFilterCallback,filter.GetPointer());
	}
	// Inicializo el combo de filtros PolyDataToPolyData
	p2pfilters=TCLFilterUtils::hasOutputType("PolyData",TCLFilterUtils::hasInputType("PolyData",TCLFilterUtils::getAvailableFilters(Config::Instance()->getFiltersDirectory())));
	for (ifit=p2pfilters.begin();ifit!=p2pfilters.end();ifit++){
		TCLFilter::Pointer filter=(*ifit);
		std::string fname=filter->getName();
		if (fname.length()>40) fname=fname.substr(0,40);
		fname=std::string(_("&Filters/&Surface/"))+fname;
		int idx=menu->add(fname.c_str(),0,(Fl_Callback*)surfaceFilterCallback,filter.GetPointer());
	}
	//
	pipeline=Pipeline::New();
	lastImageNumber=1;
	lastFMSeedNumber=1;
	currentSegmentationPanel=0;
	smMap[grVG]=VoxelGrowSegmentation::New();
	smMap[grFM]=FastMarchingSegmentation::New();
	smMap[grAC]=ActiveContourSegmentation::New();
	comboSegMethods->add(_("VoxelGrow"),0,0,grVG);
	comboSegMethods->add(_("FastMarching"),0,0,grFM);
	comboSegMethods->add(_("Active Contours"),0,0,grAC);
	vtkObject::GlobalWarningDisplayOff();
	showDesktopTab(0);
}

DesktopGUI::~DesktopGUI(){
}

void DesktopGUI::redrawVtk3D(){
	if (btnSurfacesTab->value()){
		vtkSurfaces->redraw();
	}
	else{
		vtk3D->redraw();
	}
	Fl::check();
}

bool DesktopGUI::configureFilterPipeline(TCLFilterPipeline::Pointer fp){
	TCLFilter::Vector availFilters=TCLFilterUtils::getAvailableFilters(Config::Instance()->getFiltersDirectory());
	TCLFilterPipelineConfigGUI* pcfg=new TCLFilterPipelineConfigGUI(fp,availFilters);
	bool ok=pcfg->configure();
	delete pcfg;
	return ok;
}

void DesktopGUI::updateSegmentationMethodParams(){
	SegmentationMethod* sm=smMap[currentSegmentationPanel].GetPointer();
	if (currentSegmentationPanel==grFM){
		FastMarchingSegmentation::Pointer fm=(FastMarchingSegmentation*)sm;
		VoxelModel::Vector seeds;
		for (int i=1;i<=browserFMSeeds->size();i++){
			void* d=browserFMSeeds->data(i);
			int ikey=(int)d;
			seeds.push_back(fmSeeds[ikey]);
		}
		fm->setSeeds(seeds);
		fm->setSigma(atof(FMSigma->value()));
	}
	if (currentSegmentationPanel==grVG){
		VoxelGrowSegmentation::Pointer vg=(VoxelGrowSegmentation*)sm;
		vg->setRadius(atoi(VGRadius->value()));
		vg->setConfig(currentVGConfigVector);
		vg->setStretchRadius(atoi(VGStretchRadius->value()));
	}
	if (currentSegmentationPanel==grAC){
		ActiveContourSegmentation::Pointer ac=(ActiveContourSegmentation*)sm;
		ImageModel::Pointer biimg=getNamedImage(ACBinaryImageName->value());
		if (biimg.GetPointer()){
			ac->setBinaryImage(biimg,atoi(ACBinaryObjVal->value()));
		}
	}
}

void DesktopGUI::resetCamera(){
	if (!btnSurfacesTab->value()){
		ImageModel::Pointer img=getSelectedImage();
		if (!img.GetPointer()) return;
		pipeline->getImageSegment(img)->getVtkRenderer()->ResetCamera();
	}
	else
		renSurfaces->ResetCamera();
	redrawVtk3D();
}

ImageModel::Pointer DesktopGUI::getSelectedImage(){
	int v=browserImages->value();
	if (!v){
		grImageProperties->deactivate();
		grSeedSelection->deactivate();
		if (currentSegmentationPanel) currentSegmentationPanel->deactivate();
	}
	else{
		if (currentSegmentationPanel) currentSegmentationPanel->activate();
		grImageProperties->activate();
		grSeedSelection->activate();
		return (ImageModel*)browserImages->data(v);
	}
	return 0;
}


SurfaceModel::Pointer DesktopGUI::getSelectedSurface(){
	int index=browserSurfaces->value();
	if (index==0){
		grSurfaceProperties->deactivate();
		return 0;
	}
	else{
		grSurfaceProperties->activate();
		return (SurfaceModel*)browserSurfaces->data(index);
	}
	return 0;
}


void DesktopGUI::addImage(ImageModel::Pointer img){
	vtkImageData* vol=img->getFilteredVtkVolume();
	vol->Update();
	float scalarRange[2];
	vol->GetScalarRange(scalarRange);
	//Seteo rainbow
	if (scalarRange[0]>=0) {
		img->setLUTGamma(7,scalarRange[0],scalarRange[1]-scalarRange[0]);
	}else{
		img->setLUTGamma(7,0,scalarRange[1]);
	}
	pipeline->addVolume(img,win3D);
	browserImages->add(img->getLabel().c_str(),img.GetPointer());
	browserImages->value(browserImages->size());
	imageSelected();
}

ImageModel::Pointer DesktopGUI::getNamedImage(std::string name){
	for (int i=1;i<=browserImages->size();i++){
		ImageModel::Pointer im=(ImageModel*)browserImages->data(i);
		if (im.GetPointer() && im->getLabel()==name) return im;
	}
	return 0;
}
 
void DesktopGUI::quit(){
	window->hide();
}

void DesktopGUI::focusSeed(VoxelModel focus){
	ImageModel::Pointer img=getSelectedImage();
	seedX->value(focus.x);
	seedY->value(focus.y);
	seedZ->value(focus.z);
	vtkImageData* vtkvol=img->getFilteredVtkVolume();
	int dims[3];
	vtkvol->GetDimensions(dims);
	int extent[6];
	vtkvol->GetExtent(extent);
	int pos[3]={focus.x-extent[0],focus.y-extent[2],focus.z-extent[4]};
	focus.intensity=vtkvol->GetPointData()->GetScalars()->GetComponent(vtkvol->ComputePointId(pos),0);
	seedIntensity->value(focus.intensity);
	//Magnitud de Gradiente
	float r[3];
	vtkvol->GetSpacing(r);
	r[0] = -0.5 / r[0];
	r[1] = -0.5 / r[1];
	r[2] = -0.5 / r[2];
	//X
	float gx=0;
	if (pos[0]>0 && pos[0]<dims[0]-1){
		pos[0]=pos[0]+1;
		gx = vtkvol->GetPointData()->GetScalars()->GetComponent(vtkvol->ComputePointId(pos),0);
		pos[0]=pos[0]-2;
		gx -= vtkvol->GetPointData()->GetScalars()->GetComponent(vtkvol->ComputePointId(pos),0);
		pos[0]=pos[0]+1;
		gx *= r[0]; // multiply by the data spacing
		gx = gx*gx;
	}
	//Y
	float gy=0;
	if (pos[1]>0 && pos[1]<dims[1]-1){
		pos[1]=pos[1]+1;
		gy = vtkvol->GetPointData()->GetScalars()->GetComponent(vtkvol->ComputePointId(pos),0); 
		pos[1]=pos[1]-2;
		gy -= vtkvol->GetPointData()->GetScalars()->GetComponent(vtkvol->ComputePointId(pos),0);
		pos[1]=pos[1]+1;
		gy *= r[1]; // multiply by the data spacing
		gy = gy*gy;
	}
	//Z
	float gz=0;
	if (pos[2]>0 && pos[2]<dims[2]-1){
		pos[2]=pos[2]+1;
		gz = vtkvol->GetPointData()->GetScalars()->GetComponent(vtkvol->ComputePointId(pos),0);
		pos[2]=pos[2]-2;
		gz -= vtkvol->GetPointData()->GetScalars()->GetComponent(vtkvol->ComputePointId(pos),0);
		pos[2]=pos[2]+1;
		gz *= r[2]; // multiply by the data spacing
		gz = gz*gz;
	}
	float gmag=sqrt(gx+gy+gz);
	seedGradient->value(gmag);
	//
	currentSeed=focus;
	ImageSegment::Pointer seg=pipeline->getImageSegment(img);
	if (seg.GetPointer()){
		seg->focusSeed(currentSeed);
		redrawVtk3D();
	}
}

void DesktopGUI::seedFocusChanged(){
	VoxelModel focus;
	focus.x=(int)seedX->value();
	focus.y=(int)seedY->value();
	focus.z=(int)seedZ->value();
	ImageModel::Pointer img=getSelectedImage();
	vtkImageData* vtkvol=img->getFilteredVtkVolume();
	int extent[6];
	vtkvol->GetExtent(extent);
	int pos[3]={focus.x-extent[0],focus.y-extent[2],focus.z-extent[4]};
	focus.intensity=vtkvol->GetPointData()->GetScalars()->GetComponent(vtkvol->ComputePointId(pos),0);
	seedIntensity->value(focus.intensity);
	int dims[3];
	vtkvol->GetDimensions(dims);
	//Magnitud de Gradiente
	float r[3];
	vtkvol->GetSpacing(r);
	r[0] = -0.5 / r[0];
	r[1] = -0.5 / r[1];
	r[2] = -0.5 / r[2];
	//X
	float gx=0;
	if (pos[0]>0 && pos[0]<dims[0]-1){
		pos[0]=pos[0]+1;
		gx = vtkvol->GetPointData()->GetScalars()->GetComponent(vtkvol->ComputePointId(pos),0);
		pos[0]=pos[0]-2;
		gx -= vtkvol->GetPointData()->GetScalars()->GetComponent(vtkvol->ComputePointId(pos),0);
		pos[0]=pos[0]+1;
		gx *= r[0]; // multiply by the data spacing
		gx = gx*gx;
	}
	//Y
	float gy=0;
	if (pos[1]>0 && pos[1]<dims[1]-1){
		pos[1]=pos[1]+1;
		gy = vtkvol->GetPointData()->GetScalars()->GetComponent(vtkvol->ComputePointId(pos),0); 
		pos[1]=pos[1]-2;
		gy -= vtkvol->GetPointData()->GetScalars()->GetComponent(vtkvol->ComputePointId(pos),0);
		pos[1]=pos[1]+1;
		gy *= r[1]; // multiply by the data spacing
		gy = gy*gy;
	}
	//Z
	float gz=0;
	if (pos[2]>0 && pos[2]<dims[2]-1){
		pos[2]=pos[2]+1;
		gz = vtkvol->GetPointData()->GetScalars()->GetComponent(vtkvol->ComputePointId(pos),0);
		pos[2]=pos[2]-2;
		gz -= vtkvol->GetPointData()->GetScalars()->GetComponent(vtkvol->ComputePointId(pos),0);
		pos[2]=pos[2]+1;
		gz *= r[2]; // multiply by the data spacing
		gz = gz*gz;
	}
	float gmag=sqrt(gx+gy+gz);
	seedGradient->value(gmag);
	//
	focusSeed(focus);
	redrawVtk3D();
}

//Que hace este metodo??
void DesktopGUI::imageSelected(){
	ImageModel::Pointer vol=getSelectedImage();
	if (!vol.GetPointer()) return;
	if (currentVolume.GetPointer()) pipeline->getImageSegment(currentVolume)->setVisibility(false);
	currentVolume=vol;
	pipeline->getImageSegment(currentVolume)->setVisibility(true);
	int extents[6];
	vtkImageData* vtkvol=currentVolume->getFilteredVtkVolume();
	vtkvol->GetExtent(extents);
	seedX->range(extents[0],extents[1]);
	seedX->step(1.0);
	seedY->range(extents[2],extents[3]);
	seedY->step(1.0);
	seedZ->range(extents[4],extents[5]);
	seedZ->step(1.0);
	sprintf(lblHolderDimensions,"%d x %d x %d",extents[1]-extents[0]+1,extents[3]-extents[2]+1,extents[5]-extents[4]+1);
	lblDimensions->label(lblHolderDimensions);
	lblDimensions->redraw();
	float scalarRange[2];
	vtkvol->GetScalarRange(scalarRange);
	sprintf(lblHolderColorRange,"%4.2f - %4.2f",scalarRange[0],scalarRange[1]);
	lblColorRange->label(lblHolderColorRange);
	lblColorRange->redraw(); 
	VoxelModel focus=pipeline->getImageSegment(currentVolume)->getFocusedSeed();
	focusSeed(focus);
	resetCamera();
	
	updateImageUI();

	redrawVtk3D();
}

void DesktopGUI::updateImageUI(){
	ImageModel::Pointer vol=getSelectedImage();
	if (!vol.GetPointer()) return;
	//Actualiza botonoes visibilidad plano
	ImageSegment::Pointer imgseg=pipeline->getImageSegment(vol);
	if (imgseg->getPlaneVisibility(PLANE_XY)){
		imageViewMenuXYItem->set();
		imageViewXYButton->value(1);
	}
	else{
		imageViewMenuXYItem->clear();
		imageViewXYButton->value(0);
	}
	if (imgseg->getPlaneVisibility(PLANE_XZ)){
		imageViewMenuXZItem->set();
		imageViewXZButton->value(1);
	}
	else{
		imageViewMenuXZItem->clear();
		imageViewXZButton->value(0);
	}
	if (imgseg->getPlaneVisibility(PLANE_YZ)){
		imageViewMenuYZItem->set();
		imageViewYZButton->value(1);
	}
	else{
		imageViewMenuYZItem->clear();
		imageViewYZButton->value(0);
	}
	//Actualiza gama de colores
	btnShowLUTBar->value(imgseg->getLUTBarVisibility());
	cbImageLUT->value(vol->getLUTGamma());
	char holder[1024];
	sprintf(holder,"%d",vol->getLookupTable()->GetNumberOfColors());
	imageLUTLevels->value(holder);
	//Actualiza seguimiento por contorno
	btnShowContour->value(imgseg->getContourVisibility());
}

void DesktopGUI::deleteImage(){
	ImageModel::Pointer vol=getSelectedImage();
	if (!vol.GetPointer()) return;
	browserImages->remove(browserImages->value());
	pipeline->removeVolume(vol);
	currentVolume=0;
	imageSelected();
	redrawVtk3D();
}

void DesktopGUI::editImageFilters(){
	ImageModel::Pointer vol=getSelectedImage();
	if (!vol.GetPointer()) return;
	if (configureFilterPipeline(vol->getFiltersPipeline())){
		vol->getFilteredVtkVolume()->Update();
		imageSelected();
		redrawVtk3D();
	}
}

void DesktopGUI::setCutPlaneVisibility(PlaneType plane,bool visible){
	ImageModel::Pointer vol=getSelectedImage();
	if (!vol.GetPointer()) return;
	ImageSegment::Pointer imgseg=pipeline->getImageSegment(vol);
	imgseg->setPlaneVisibility(plane,visible);
	updateImageUI();
	redrawVtk3D();
}

void DesktopGUI::editImageSceneFilters(){
	ImageModel::Pointer vol=getSelectedImage();
	if (!vol.GetPointer()) return;
	if (configureFilterPipeline(vol->getScenePipeline())){
		imageSelected();
		redrawVtk3D();
	}
}

void DesktopGUI::segmentationMethodChanged(){
	if (currentSegmentationPanel) currentSegmentationPanel->hide();
	currentSegmentationPanel=(Fl_Group*)comboSegMethods->mvalue()->user_data();
	currentSegmentationPanel->show();
}

void DesktopGUI::VGSeedSelected(){
	int v=browserVGSeeds->value();
	if (!v){
		btnVGremoveSeed->deactivate();
		grVGConfig->deactivate();
	}
	else{
		btnVGremoveSeed->activate();
		grVGConfig->activate();
		VoxelGrow::ConfigPair cfg=currentVGConfigVector[v-1];
		char holder[255];
		sprintf(holder,"%4.2f",cfg.second->getDelta());
		VGDelta->value(holder);
		int type=(int)browserVGSeeds->data(v);
		VGType->value(type);
		focusSeed(cfg.first);
	}
}

void DesktopGUI::VGAddSeed(){
	VoxelGrow::ConfigPair cfg;
	cfg.first=currentSeed;
	int type=VGType->value();
	if (type==0) cfg.second=SeedDeltaProcessor::New().GetPointer();
	if (type==1) cfg.second=NeighborDeltaProcessor::New().GetPointer();
	if (type==2) cfg.second=GradientProcessor::New().GetPointer();
	cfg.second->setDelta(atof(VGDelta->value()));
	currentVGConfigVector.push_back(cfg);
	char holder[255];
	sprintf(holder,"%d %d %d",cfg.first.x,cfg.first.y,cfg.first.z);
	browserVGSeeds->add(holder,(void*)type);
	VGSeedSelected();
}

void DesktopGUI::VGRemoveSeed(){
	int v=browserVGSeeds->value();
	VoxelGrow::ConfigVector::iterator i=currentVGConfigVector.begin()+(v-1);
	currentVGConfigVector.erase(i);
	browserVGSeeds->remove(v);
	VGSeedSelected();
}

void DesktopGUI::VGUpdateConfig(){
	int v=browserVGSeeds->value();
	VoxelGrow::ConfigPair cfg=currentVGConfigVector[v-1];
	int type=VGType->value();
	if (type==0) cfg.second=SeedDeltaProcessor::New().GetPointer();
	if (type==1) cfg.second=NeighborDeltaProcessor::New().GetPointer();
	if (type==2) cfg.second=GradientProcessor::New().GetPointer();
	cfg.second->setDelta(atof(VGDelta->value()));
	browserVGSeeds->data(v,(void*)type);
	currentVGConfigVector[v-1]=cfg;
}

void DesktopGUI::doSegmentation(){
	ImageModel::Pointer vol=getSelectedImage();
	if (!vol.GetPointer()) return;
	SegmentationMethod::Pointer sm=smMap[currentSegmentationPanel];
	if (!sm.GetPointer()) return;
	char defname[255];
	sprintf(defname,_("Segmentation no.%d"),lastImageNumber);
	const char* rl=fl_input(_("Enter a segmentation label"),defname);
	if (!rl) return;
	updateSegmentationMethodParams();
	Segmentation::Pointer seg=sm->segmentate(vol);
	if (seg->haveImage()){
		ImageModel::Pointer smout=seg->getImage();
		smout->setLabel(rl);
		addImage(smout);
		VoxelModel vm;
		int dims[3];
		smout->getFilteredVtkVolume()->GetDimensions(dims);
		vm.x = dims[0]/2;
		vm.y = dims[1]/2;
		vm.z = dims[2]/2;
		pipeline->getImageSegment(smout)->focusSeed(vm);		
	}
	if (seg->haveSurface()){
		SurfaceModel::Pointer smout=seg->getSurface();
		smout->setLabel(rl);
		addSurface(smout);
	}
}

void DesktopGUI::addSurface(SurfaceModel::Pointer surface){
	pipeline->addSurface(surface,renSurfaces);
	browserSurfaces->add(surface->getLabel().c_str(),surface.GetPointer());
	browserSurfaces->value(browserSurfaces->size());
	showDesktopTab(2);
	resetCamera();
}

void DesktopGUI::createMarchingCubesSurface(){
	ImageModel::Pointer vol=getSelectedImage();
	if (!vol.GetPointer()) return;
	char defcolor[255];
	sprintf(defcolor,"%4.2f",currentSeed.intensity);
	const char* rl=fl_input(_("Enter cut color"),defcolor);
	if (!rl) return;
	float cutcolor=atof(rl);
	
	SurfaceModel::Pointer surface=SurfaceModel::New();
	surface->setLabel(vol->getLabel());
	vtkMarchingCubes* mc=vtkMarchingCubes::New();
	mc->SetInput(vol->getFilteredVtkVolume());
	mc->SetValue(0,cutcolor);
	mc->SetOutput(surface->getInputPolyData());
	mc->ComputeGradientsOff();
	mc->ComputeScalarsOn();
	mc->ComputeNormalsOn();
	ProgressWindowGUI::Instance()->Observe(mc,_("Building surface using Marching Cubes Algorithm"),vol->getLabel());
	mc->Update();
	//Desconecto la salida y la entrada
	vtkPolyData* fakeP=vtkPolyData::New();
	mc->SetOutput(fakeP);
	fakeP->Delete();
	vtkImageData* fakeI=vtkImageData::New();
	mc->SetInput(fakeI);
	fakeI->Delete();
	//
	mc->Delete();
	addSurface(surface);
}

void DesktopGUI::createFlatContourSurface(){
	ImageModel::Pointer vol=getSelectedImage();
	if (!vol.GetPointer()) return;
	const char* rl=fl_input(_("Enter outside color"),"0.0");
	if (!rl) return;
	float cutcolor=atof(rl);
	SurfaceModel::Pointer surface=SurfaceModel::New();
	surface->setLabel(vol->getLabel());
	FlatContour* fc=new FlatContour();
	fc->SetInput(vol->getFilteredVtkVolume());
	fc->setOutsideValue(cutcolor);
	ProgressWindowGUI::Instance()->Observe(fc,_("Building surface using FlatContour Algorithm"),vol->getLabel());
	fc->SetOutput(surface->getInputPolyData());
	fc->Update();
	//Desconecto la salida y la entrada
	vtkPolyData* fakeP=vtkPolyData::New();
	fc->SetOutput(fakeP);
	fakeP->Delete();
	vtkImageData* fakeI=vtkImageData::New();
	fc->SetInput(fakeI);
	fakeI->Delete();
	//
	fc->Delete();
	addSurface(surface);

}


void DesktopGUI::surfaceSelected(){
	SurfaceModel::Pointer sm=getSelectedSurface();
	if (sm.GetPointer()==0) return;
	vtkActor* a=pipeline->getSurfaceSegment(sm)->getSurfaceActor();
	cbVisibleSurface->value(a->GetVisibility());
	cbWireframeSurface->value((a->GetProperty()->GetRepresentation()==VTK_WIREFRAME));
	float rgb[3];
	a->GetProperty()->GetColor(rgb);
	btnSurfaceColor->color(fl_rgb_color((unsigned char)(rgb[0]*255),(unsigned char)(rgb[1]*255),(unsigned char)(rgb[2]*255)));
	btnSurfaceColor->redraw();
	sliderSurfaceTransparency->minimum(0);
	sliderSurfaceTransparency->maximum(1.0);
	sliderSurfaceTransparency->step(0.01);
	sliderSurfaceTransparency->value(1.0-a->GetProperty()->GetOpacity());	
}

void DesktopGUI::deleteSurface(){
	SurfaceModel::Pointer sm=getSelectedSurface();
	if (sm.GetPointer()==0) return;
	pipeline->removeSurface(sm);
	browserSurfaces->remove(browserSurfaces->value());
	if (browserSurfaces->value()<1) browserSurfaces->value(1);
	surfaceSelected();
	redrawVtk3D();
}

void DesktopGUI::editSurfaceFilters(){
	SurfaceModel::Pointer surface=getSelectedSurface();
	if (!surface.GetPointer()) return;
	if (configureFilterPipeline(surface->getFilterPipeline())){
		surface->getFilteredPolyData()->Update();
		surfaceSelected();
		redrawVtk3D();
	}
}

void DesktopGUI::changeVisibleSurfaceProperty(){
	SurfaceModel::Pointer sm=getSelectedSurface();
	if (sm.GetPointer()==0) return;
	vtkActor* a=pipeline->getSurfaceSegment(sm)->getSurfaceActor();
	a->SetVisibility(cbVisibleSurface->value());
	redrawVtk3D();
}

void DesktopGUI::changeWireframeSurfaceProperty(){
	SurfaceModel::Pointer sm=getSelectedSurface();
	if (sm.GetPointer()==0) return;
	vtkActor* a=pipeline->getSurfaceSegment(sm)->getSurfaceActor();
	if (cbWireframeSurface->value()) a->GetProperty()->SetRepresentationToWireframe();
	else
		a->GetProperty()->SetRepresentationToSurface();
	redrawVtk3D();
}

void DesktopGUI::changeColorSurfaceProperty(){
	SurfaceModel::Pointer sm=getSelectedSurface();
	if (sm.GetPointer()==0) return;
	float origrgb[3];
	vtkActor* a=pipeline->getSurfaceSegment(sm)->getSurfaceActor();
	a->GetProperty()->GetColor(origrgb);
	double rgb[]={origrgb[0],origrgb[1],origrgb[2]};
	if (fl_color_chooser(_("Select surface color"),rgb[0],rgb[1],rgb[2])){
		origrgb[0]=rgb[0];
		origrgb[1]=rgb[1];
		origrgb[2]=rgb[2];
		a->GetProperty()->SetColor(origrgb);
		btnSurfaceColor->color(fl_rgb_color((unsigned char)(origrgb[0]*255),(unsigned char)(origrgb[1]*255),(unsigned char)(origrgb[2]*255)));
		btnSurfaceColor->redraw();
	}
	redrawVtk3D();
}


void DesktopGUI::changeTransparencySurfaceProperty(){
	SurfaceModel::Pointer sm=getSelectedSurface();
	if (sm.GetPointer()==0) return;
	vtkActor* a=pipeline->getSurfaceSegment(sm)->getSurfaceActor();
	a->GetProperty()->SetOpacity(1.0-sliderSurfaceTransparency->value());
	redrawVtk3D();
}

void DesktopGUI::FMAddSeed(){
	lastFMSeedNumber++;
	fmSeeds[lastFMSeedNumber]=currentSeed;
	char holder[255];
	sprintf(holder,"%d %d %d",currentSeed.x,currentSeed.y,currentSeed.z);
	browserFMSeeds->add(holder,(void*)lastFMSeedNumber);
	btnFMremoveSeed->activate();
}

void DesktopGUI::FMRemoveSeed(){
	int index=browserFMSeeds->value();
	if (!index) return;
	int ikey=*(int*)browserFMSeeds->data(index);
	browserFMSeeds->remove(index);
	fmSeeds.erase(ikey);
	if (browserFMSeeds->size()==0) btnFMremoveSeed->deactivate();
}

#if (VTK_MAJOR_VERSION == 4 && VTK_MINOR_VERSION > 0)
void DesktopGUI::OnChar(){
}
void DesktopGUI::OnLeftButtonDown(){
	vtkInteractorStyleSwitch::OnLeftButtonDown();
	/** Must see how to do with VTK 4.x
	if (!ctrl || !shift) vtkInteractorStyleSwitch::OnLeftButtonDown();
	else{
		if (Interactor==vtk3D && CurrentRenderer){
			ImageModel::Pointer img=getSelectedImage();
			if (!img.GetPointer()) return;
			float spacing[3];
			float origin[3];
			int extents[6];
			img->getFilteredVtkVolume()->GetSpacing(spacing);
			img->getFilteredVtkVolume()->GetOrigin(origin);
			img->getFilteredVtkVolume()->GetExtent(extents);
			vtk3D->GetPicker()->Pick(X,Y,0.0,CurrentRenderer);
			float position[3];
			vtk3D->GetPicker()->GetPickPosition(position);
			int spos[3]={(int)(position[0]/spacing[0]+origin[0]),(int)(position[1]/spacing[1]+origin[1]),(int)(position[2]/spacing[2]+origin[2])};
			if (spos[0]>=extents[0] && spos[0]<=extents[1] &&
				spos[1]>=extents[2] && spos[1]<=extents[3] &&
				spos[2]>=extents[4] && spos[2]<=extents[5]){
					VoxelModel selSeed;
					selSeed.x=spos[0];
					selSeed.y=spos[1];
					selSeed.z=spos[2];
					focusSeed(selSeed);
				}
		}
	}
	*/
}
#else
void DesktopGUI::OnChar(int ctrl, int shift, char keycode, int repeatcount){
}

void DesktopGUI::OnLeftButtonDown(int ctrl, int shift, int X, int Y){
	if (!ctrl || !shift) vtkInteractorStyleSwitch::OnLeftButtonDown(ctrl,shift,X,Y);
	else{
		this->UpdateInternalState(ctrl, shift, X, Y);
		this->FindPokedCamera(X, Y);
		this->FindPokedRenderer(X,Y);
		if (Interactor==vtk3D && CurrentRenderer){
			ImageModel::Pointer img=getSelectedImage();
			if (!img.GetPointer()) return;
			float spacing[3];
			float origin[3];
			int extents[6];
			img->getFilteredVtkVolume()->GetSpacing(spacing);
			img->getFilteredVtkVolume()->GetOrigin(origin);
			img->getFilteredVtkVolume()->GetExtent(extents);
			vtk3D->GetPicker()->Pick(X,Y,0.0,CurrentRenderer);
			float position[3];
			vtk3D->GetPicker()->GetPickPosition(position);
			int spos[3]={(int)(position[0]/spacing[0]+origin[0]),(int)(position[1]/spacing[1]+origin[1]),(int)(position[2]/spacing[2]+origin[2])};
			if (spos[0]>=extents[0] && spos[0]<=extents[1] &&
				spos[1]>=extents[2] && spos[1]<=extents[3] &&
				spos[2]>=extents[4] && spos[2]<=extents[5]){
					VoxelModel selSeed;
					selSeed.x=spos[0];
					selSeed.y=spos[1];
					selSeed.z=spos[2];
					focusSeed(selSeed);
				}
		}
	}
}
#endif

void DesktopGUI::setSurfaceTexture(){
	SurfaceModel::Pointer sm=getSelectedSurface();
	if (sm.GetPointer()==0) return;
	char* fname="";
	fname=fl_file_chooser(_("Select texture image"),_("Images (*.{bmp,jpg,jpeg,png})"),"");
	if (!fname) return;
	std::string filename=fname;
	std::string extension;
	int pos=filename.find_last_of(".");
	if (pos!=std::string::npos){
		extension=filename.substr(pos+1,255);
	}
	std::transform(extension.begin(),extension.end(),extension.begin(),tolower);
	vtkImageData* timg=vtkImageData::New();
	if (extension=="bmp"){
		vtkBMPReader* bmpr=vtkBMPReader::New();
		bmpr->SetFileName(fname);
		ProgressWindowGUI::Instance()->Observe(bmpr,_("Reading texture image..."),sm->getLabel());
		bmpr->SetOutput(timg);
		bmpr->Update();
		bmpr->Delete();
	}
	else
	if (extension=="jpeg" || extension=="jpg"){
		vtkJPEGReader* jpr=vtkJPEGReader::New();
		jpr->SetFileName(fname);
		ProgressWindowGUI::Instance()->Observe(jpr,_("Reading texture image..."),sm->getLabel());
		jpr->SetOutput(timg);
		jpr->Update();
		jpr->Delete();
	}
	else
	if (extension=="png"){
		vtkPNGReader* pnr=vtkPNGReader::New();
		pnr->SetFileName(fname);
		ProgressWindowGUI::Instance()->Observe(pnr,_("Reading texture image..."),sm->getLabel());
		pnr->SetOutput(timg);
		pnr->Update();
		pnr->Delete();
	}
	pipeline->getSurfaceSegment(sm)->setTexture(timg);
	timg->Delete();
	redrawVtk3D();
}

void DesktopGUI::clearSurfaceTexture(){
	SurfaceModel::Pointer sm=getSelectedSurface();
	if (sm.GetPointer()==0) return;
	pipeline->getSurfaceSegment(sm)->clearTexture();
	redrawVtk3D();
}

void DesktopGUI::getMetrics(){
	SurfaceModel::Pointer sm=getSelectedSurface();
	if (sm.GetPointer()==0) return;
	MetricsGUI* mg=new MetricsGUI();
	mg->show(sm);
	delete mg;
}

void DesktopGUI::renameImage(){
	ImageModel::Pointer i=getSelectedImage();
	if (!i.GetPointer()) return;
	const char* newname=fl_input(_("Enter the image´s new name"),i->getLabel().c_str());
	if (!newname) return;
	i->setLabel(newname);
	browserImages->text(browserImages->value(),newname);
	browserImages->redraw();
}

void DesktopGUI::renameSurface(){
	SurfaceModel::Pointer s=getSelectedSurface();
	if (!s.GetPointer()) return;
	const char* newname=fl_input(_("Enter the surface´s new name"),s->getLabel().c_str());
	if (!newname) return;
	s->setLabel(newname);
	browserSurfaces->text(browserSurfaces->value(),newname);
	browserSurfaces->redraw();
}

void DesktopGUI::editSurfaceSceneFilters(){
	SurfaceModel::Pointer s=getSelectedSurface();
	if (!s.GetPointer()) return;
	if (configureFilterPipeline(s->getScenePipeline())){
		surfaceSelected();
		redrawVtk3D();
	}
}

void DesktopGUI::load3DImage(){
	const char* fcr=fl_file_chooser(_("Load 3D Image"),_("3D Images(*.{spb,spt,sp})"),lastImageVolumeFile.c_str(),0);
	if (!fcr) return;
	vtkStructuredPointsReader* dsr=vtkStructuredPointsReader::New();
	dsr->SetFileName(fcr);
	ProgressWindowGUI::Instance()->doStartEvent(_("Reading 3D Image..."));
	ImageModel::Pointer vol=ImageModel::New();
	vol->setLabel(fl_filename_name(fcr));
	dsr->SetOutput(vtkStructuredPoints::SafeDownCast(vol->getInputVtkVolume()));
	dsr->Update();
	addImage(vol);
	dsr->Delete();
	ProgressWindowGUI::Instance()->doEndEvent();
}


void DesktopGUI::showDICOMBrowser(){
	const char* dcr=fl_dir_chooser(_("Load DICOM"),NULL,0);
	DicomBrowserGUI *dicomB=new DicomBrowserGUI(dcr);
	bool ok;
	if (!dcr) return;
	else{
		ok = dicomB->exec();
		if (ok){
			ImageModel::Pointer vol=dicomB->readVolume();
			if (vol.GetPointer()) addImage(vol);
		}
	}
	delete dicomB;
}

void DesktopGUI::save3DImage(){
	ImageModel::Pointer vol=getSelectedImage();
	if (!vol.GetPointer()) return;
	char* fname="";
	fname=fl_file_chooser(_("Save 3D Image"),_("3D Images (*.{spb,spt})"),"");
	if (!fname) return;
	std::string filename=fl_filename_name(fname);
	std::string extension;
	int pos=filename.find_last_of(".");
	if (pos!=std::string::npos){
		extension=filename.substr(pos+1,255);
	}
	vtkStructuredPointsWriter* spw=vtkStructuredPointsWriter::New();
	spw->SetInput(vol->getFilteredVtkVolume());
	spw->SetFileName(fname);
	if (extension=="spt") spw->SetFileTypeToASCII();	
	else
		spw->SetFileTypeToBinary();
	ProgressWindowGUI::Instance()->doStartEvent(_("Saving 3D Image..."));
	spw->Write();
	ProgressWindowGUI::Instance()->doEndEvent();
	spw->Delete();
}

void DesktopGUI::loadSurface(){
	char* fcr=fl_file_chooser(_("Load Surface"),_("Surfaces (*.{pdb,pdt,pd})"),"",0);
	//char* fcr = "E:/SkullyDoo/Data/ct1-skull.pdb";
	if (!fcr) return;
	vtkPolyDataReader* dsr=vtkPolyDataReader::New();
	SurfaceModel::Pointer surface=SurfaceModel::New();
	surface->setLabel(fl_filename_name(fcr));
	dsr->SetFileName(fcr);
	dsr->SetOutput(surface->getInputPolyData());
	ProgressWindowGUI::Instance()->doStartEvent(_("Reading surface..."));
	dsr->Update();
	ProgressWindowGUI::Instance()->doEndEvent();
	addSurface(surface);
	dsr->Delete();
}

void DesktopGUI::saveSurface(){
	SurfaceModel::Pointer sm=getSelectedSurface();
	if (sm.GetPointer()==0) return;
	char* fname="";
	fname=fl_file_chooser(_("Save Surface"),_("Surfaces (*.{pdb,pdt})"),"");
	if (!fname) return;
	std::string filename=fl_filename_name(fname);
	std::string extension;
	int pos=filename.find_last_of(".");
	if (pos!=std::string::npos){
		extension=filename.substr(pos+1,255);
	}
	std::transform(extension.begin(),extension.end(),extension.begin(),tolower);
	vtkPolyDataWriter* spw=vtkPolyDataWriter::New();
	if (extension=="pdt") spw->SetFileTypeToASCII();
	else
		spw->SetFileTypeToBinary();
	spw->SetInput(sm->getFilteredPolyData());
	spw->SetFileName(fname);
	ProgressWindowGUI::Instance()->doStartEvent(_("Saving surface..."));
	spw->Write();
	ProgressWindowGUI::Instance()->doEndEvent();
	spw->Delete();
}

void DesktopGUI::import3DImage(){
	const char* fcr=fl_file_chooser(_("Select file"),"*.*",lastImageVolumeFile.c_str(),0);
	if (!fcr) return;
	VolumeLoaderGUI* vlg=new VolumeLoaderGUI(fcr);
	bool ok=vlg->execute();
	if (ok){
		lastImageVolumeFile=fcr;
		ImageModel::Pointer vol=vlg->readVolume();
		if (vol.GetPointer()) addImage(vol);
	}
	delete vlg;
}

void DesktopGUI::importSURSurface(){
	const char* fcr=fl_file_chooser(_("Import .SUR"),"*.sur","",0);
	if (!fcr) return;
	SurfaceModel::Pointer surface=SurfaceModel::New();
	if (SURFormatIO::read(fcr,surface->getInputPolyData())){
		surface->setLabel(fl_filename_name(fcr));
		addSurface(surface);
	}
}

void DesktopGUI::importSTLSurface(){
	const char* fcr=fl_file_chooser(_("Import .STL"),"*.stl","",0);
	if (!fcr) return;
	SurfaceModel::Pointer surface=SurfaceModel::New();
	vtkSTLReader *wrt = vtkSTLReader::New();
	if (!fcr)return;
	wrt->SetFileName(fcr);
	wrt->SetOutput( surface->getInputPolyData());
	surface->setLabel(fl_filename_name(fcr));
	addSurface(surface);
	wrt->Modified();
	wrt->Delete();
}

void DesktopGUI::exportSURSurface(){
	SurfaceModel::Pointer sm=getSelectedSurface();
	if (sm.GetPointer()==0) return;
	char* fname="";
	fname=fl_file_chooser(_("Export .SUR"),"*.sur","");
	if (!fname) return;
	if (!SURFormatIO::write(fname,sm->getFilteredPolyData())){
		std::string error=std::string(_("Error saving file "))+fname;
		fl_alert(error.c_str());
	}
}

void DesktopGUI::exportSTLSurface(){
	SurfaceModel::Pointer sm=getSelectedSurface();
	vtkTriangleFilter *triFilter = vtkTriangleFilter::New();
	if (sm.GetPointer()==0) return;
	char* fname="";
	fname=fl_file_chooser(_("Export .STL"),"*.stl","");
	triFilter->SetInput(sm->getFilteredPolyData());
	vtkSTLWriter *wrt = vtkSTLWriter::New();
	wrt->SetInput(triFilter->GetOutput());
	wrt->SetFileName(fname);
	if (!fname) return;
	wrt->Write();
	if (false){
		std::string error=std::string(_("Error saving file "))+fname;
		fl_alert(error.c_str());
	}
	triFilter->Delete();
	wrt->Delete();
}

void DesktopGUI::takeScreenSnapshot(){
	char* fname="";
	fname=fl_file_chooser(_("Exportar .JPG"),_("Imagenes JPG (*.{jpeg,jpg})"),"");
	if (!fname) return;
	vtkWindowToImageFilter* w2i=vtkWindowToImageFilter::New();
	if (btnSurfacesTab->value()) w2i->SetInput(vtkSurfaces->GetRenderWindow());
	else
		w2i->SetInput(vtk3D->GetRenderWindow());
	vtkJPEGWriter* jpw=vtkJPEGWriter::New();
	jpw->SetFileName(fname);
	jpw->SetInput(w2i->GetOutput());
	jpw->Write();
	jpw->Delete();
	w2i->Delete();
}

void DesktopGUI::changeImageLUT(){
	ImageModel::Pointer img=getSelectedImage();
	if (!img.GetPointer()) return;
	img->setLUTGamma(cbImageLUT->value(),atoi(imageLUTStart->value()),atoi(imageLUTLevels->value()));
	redrawVtk3D();
}

void DesktopGUI::applyImageToImageFilter(TCLFilter::Pointer selected){
	ImageModel::Pointer vol=getSelectedImage();
	if (!vol.GetPointer()) return;
	TCLFilterPipeline::Pointer filterpipeline=vol->getFiltersPipeline();
	filterpipeline->enablePredefinedObjects();
	TCLFilter::Pointer filter=TCLFilter::New();
	filter->setFileName(selected->getFileName());
	bool ok=filter->initialize();
	if (!ok){
		fl_alert(_("Failed to initialize filter. Error description:\n [%s]"),filter->getTCLError().c_str());
		filterpipeline->disablePredefinedObjects();
		return;
	}
	TCLFilterConfigGUI* filter_cfg=new TCLFilterConfigGUI(filter);
	filter_cfg->configure();
	delete filter_cfg;
	
	TCLFilter::Vector prevFilters=filterpipeline->getFilters();
	prevFilters.push_back(filter);
	filterpipeline->setFilters(prevFilters);
	TCLFilterPipeline::ExecStatus status=filterpipeline->execute();
	if (status==TCLFilterPipeline::EXEC_ERROR){
		fl_alert(_("TCL Error:\n%s"),filterpipeline->getErrorMessage().c_str());
		ok=false;
	}
	if (status==TCLFilterPipeline::EXEC_MISSED_OUTPUT){
		fl_alert(_("Expected Output with type %s. Check scripts (filter_output variable)"),filterpipeline->getOutputType().c_str());
		ok=false;
	}
	filterpipeline->disablePredefinedObjects();
	vol->getFilteredVtkVolume()->Update();
	imageSelected();
	redrawVtk3D();
}

void DesktopGUI::showLUTBar(bool show){
	ImageModel::Pointer img=getSelectedImage();
	if (!img.GetPointer()) return;
	pipeline->getImageSegment(img)->setLUTBarVisibility(show);
	redrawVtk3D();
}

void DesktopGUI::applyPolyDataToPolyDataFilter(TCLFilter::Pointer selected){
	SurfaceModel::Pointer surface=getSelectedSurface();
	if (!surface.GetPointer()) return;
	TCLFilterPipeline::Pointer filterpipeline=surface->getFilterPipeline();
	filterpipeline->enablePredefinedObjects();
	TCLFilter::Pointer filter=TCLFilter::New();
	filter->setFileName(selected->getFileName());
	bool ok=filter->initialize();
	if (!ok){
		fl_alert(_("Failed to initialize filter. Error:\n [%s]"),filter->getTCLError().c_str());
		filterpipeline->disablePredefinedObjects();
		return;
	}
	TCLFilterConfigGUI* filter_cfg=new TCLFilterConfigGUI(filter);
	filter_cfg->configure();
	delete filter_cfg;
	
	TCLFilter::Vector prevFilters=filterpipeline->getFilters();
	prevFilters.push_back(filter);
	filterpipeline->setFilters(prevFilters);
	TCLFilterPipeline::ExecStatus status=filterpipeline->execute();
	if (status==TCLFilterPipeline::EXEC_ERROR){
		fl_alert(_("TCL Error:\n%s"),filterpipeline->getErrorMessage().c_str());
		ok=false;
	}
	if (status==TCLFilterPipeline::EXEC_MISSED_OUTPUT){
		fl_alert(_("Expected output with type %s. Check scripts (filter_output variable)"),filterpipeline->getOutputType().c_str());
		ok=false;
	}
	filterpipeline->disablePredefinedObjects();
	surface->getFilteredPolyData()->Update();
	surfaceSelected();
	redrawVtk3D();
}


void DesktopGUI::showDesktopTab(int tab){
	if (tab==0){
		filterImage3DMenu->activate();
		filterSurfaceMenu->deactivate();
		imageViewMenu->activate();
		surfaceViewMenu->deactivate();
		grImageToolbar->activate();
		btnImage3DTab->setonly();
		grSurfaces->hide();
		grSegmentation->hide();
		grImage->show();
		grImage3D->show();
	}
	else
	if (tab==1){
		filterImage3DMenu->activate();
		filterSurfaceMenu->deactivate();
		imageViewMenu->activate();
		surfaceViewMenu->deactivate();
		grImageToolbar->activate();
		btnSegmentationTab->setonly();
		grSurfaces->hide();
		grImage3D->hide();
		grImage->show();
		grSegmentation->show();
	}
	else
	if (tab==2){
		filterImage3DMenu->deactivate();
		filterSurfaceMenu->activate();
		imageViewMenu->deactivate();
		surfaceViewMenu->activate();
		grImageToolbar->deactivate();
		btnSurfacesTab->setonly();
		grImage->hide();
		grSurfaces->show();
	}
}

void DesktopGUI::showContour(bool show){
	ImageModel::Pointer img=getSelectedImage();
	if (!img.GetPointer()) return;
	if (show){
		pipeline->getImageSegment(img)->setContourValue(seedIntensity->value());
	}
	pipeline->getImageSegment(img)->setContourVisibility(show);
	redrawVtk3D();
}

void DesktopGUI::showAbout(){
	AboutGUI* ag=new AboutGUI();
	ag->show();
	delete ag;
}

void DesktopGUI::exec(){
#ifdef _WIN32
	window->icon((char*)LoadIcon(fl_display, MAKEINTRESOURCE(IDI_APPICON)));
#endif
#if !defined(WIN32) && !defined(__APPLE__)
//	Pixmap p = XCreateBitmapFromData(fl_display, DefaultRootWindow(fl_display),
//                                 	 (const char*)skull_xpm,32, 32);
	
//	window->icon((char *)p);
#endif
	window->show(0,0);
//	window->show();
	redrawVtk3D();
}

Fl_Window* DesktopGUI::getWindow(){
	return window;
}

