/*
# $Id: DesktopGUI.h,v 1.7 2004/09/01 11:48:47 nacholarrabide Exp $
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

#ifndef _DesktopGUIBase_h_
#define _DesktopGUIBase_h_
#ifdef _MSC_VER
	#pragma warning  ( disable : 4786 )
#endif

#include "pipeline/Pipeline.h"
#include "segmentation/SegmentationMethod.h"
#include "segmentation/voxelgrow/VoxelGrow.h"
#include "tcl/TCLFilter.h"
#include "gui/DicomBrowserGUI.h"
#include "DesktopGUIBase.h"
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkInteractorStyleSwitch.h>
#include <string>
#include <map>


class DesktopGUI:protected DesktopGUIBase,protected vtkInteractorStyleSwitch{
protected:
	typedef std::map<Fl_Group*,SegmentationMethod::Pointer> SMMapType;
	typedef std::map<int,VoxelModel> SeedsMapType;

	vtkRenderWindow* win3D;
	vtkRenderer* renSurfaces;
	vtkRenderWindow* winSurfaces;

	SMMapType smMap;
	SeedsMapType fmSeeds;
	int lastImageNumber;
	int lastFMSeedNumber;
	Pipeline::Pointer pipeline;
	ImageModel::Pointer currentVolume;
	VoxelModel currentSeed;
	std::string lastImageVolumeFile;
	VoxelGrow::ConfigVector currentVGConfigVector;
	Fl_Group* currentSegmentationPanel;

	char lblHolderColorRange[1024];
	char lblHolderDimensions[1024];

	TCLFilter::Vector i2ifilters;
	TCLFilter::Vector p2pfilters;

	void updateImageUI();

	bool configureFilterPipeline(TCLFilterPipeline::Pointer fp);

	ImageModel::Pointer getSelectedImage();

	SurfaceModel::Pointer getSelectedSurface();

	void focusSeed(VoxelModel focus);

	void updateSegmentationMethodParams();

	void addImage(ImageModel::Pointer img);

	void addSurface(SurfaceModel::Pointer surface);

	virtual void quit();

	virtual void resetCamera();

	//Es llamado cuando se cambia la posicion del voxel desde algun spinbox
	virtual void seedFocusChanged();

	virtual void imageSelected();

	virtual void deleteImage();

	virtual void editImageFilters();

	virtual void setCutPlaneVisibility(PlaneType plane,bool visible);

	virtual void editImageSceneFilters();

	virtual void segmentationMethodChanged();

	virtual void VGSeedSelected();

	virtual void VGAddSeed();

	virtual void VGRemoveSeed();

	virtual void VGUpdateConfig();

	virtual void doSegmentation();

	virtual void createMarchingCubesSurface();

	virtual void createFlatContourSurface();

	virtual void surfaceSelected();

	virtual void deleteSurface();

	virtual void editSurfaceFilters();

	virtual void changeVisibleSurfaceProperty();

	virtual void changeWireframeSurfaceProperty();

	virtual void changeColorSurfaceProperty();

	virtual void changeTransparencySurfaceProperty();

	virtual void FMAddSeed();

	virtual void FMRemoveSeed();

	virtual void setSurfaceTexture();

	virtual void clearSurfaceTexture();

	virtual void getMetrics();

	virtual void renameImage();

	virtual void renameSurface();

	virtual void editSurfaceSceneFilters();

	virtual void load3DImage();
	//cargaríamos imagenes DICOM.
	virtual void showDICOMBrowser();

	virtual void save3DImage();

	virtual void loadSurface();

	virtual void saveSurface();

	virtual void import3DImage();

	virtual void importSURSurface();

	virtual void importSTLSurface();

	virtual void exportSURSurface();

	virtual void exportSTLSurface();

	virtual void takeScreenSnapshot();

	virtual void changeImageLUT();
	
	virtual void showLUTBar(bool show);

	virtual void showDesktopTab(int tab);

	virtual void showContour(bool show);

	virtual void showAbout();
	//Del interactor
#if (VTK_MAJOR_VERSION == 4 && VTK_MINOR_VERSION > 0)
	virtual void OnLeftButtonDown();
	virtual void OnChar();
#else
	virtual void OnLeftButtonDown(int ctrl, int shift, int X, int Y);
	virtual void OnChar(int ctrl, int shift, char keycode, int repeatcount);
#endif
	//Singleton
	DesktopGUI();

	static DesktopGUI* instance;

public:
	static DesktopGUI* Instance();

	virtual ~DesktopGUI();

	Fl_Window* getWindow();

	ImageModel::Pointer getNamedImage(std::string name);

	//Hace un redraw unicamente al widget seleccionado en el tab
	void redrawVtk3D();

	void applyImageToImageFilter(TCLFilter::Pointer selected);
	
	void applyPolyDataToPolyDataFilter(TCLFilter::Pointer selected);

	void exec();
};

#endif
