# $Id: CMakeLists.txt,v 1.1 2003/05/02 22:21:52 sebasfiorent Exp $
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

# Makefile del codigo fuente

IF (WIN32)
  INCLUDE (${CMAKE_ROOT}/Templates/CMakeWindowsSystemConfig.cmake)
ENDIF (WIN32)

#
# Parte de uso de TCL
#

INCLUDE (${CMAKE_ROOT}/Modules/FindTCL.cmake)
INCLUDE_DIRECTORIES(
  ${TK_XLIB_PAT}
  ${TK_INCLUDE_PATH}
  ${TCL_INCLUDE_PATH}
)
LINK_LIBRARIES (
  ${TCL_LIBRARY}
  ${TK_LIBRARY}
)

LINK_LIBRARIES (
  vtkImagingTCL
  vtkGraphicsTCL
  vtkIOTCL
  vtkFilteringTCL 
  vtkCommonTCL  
  vtkPatentedTCL
  vtkParallelTCL
  vtkHybridTCL
  vtkRenderingTCL
)

#
# Parte de uso de FLTK
#

INCLUDE (${CMAKE_ROOT}/Modules/FindFLTK.cmake)
INCLUDE_DIRECTORIES(
  ${FLTK_INCLUDE_PATH}
)
LINK_LIBRARIES(
  ${FLTK_LIBRARY}
)
IF (WIN32)
	LINK_LIBRARIES(wsock32)
ENDIF (WIN32)

#
# Parte de uso de VTK
#

INCLUDE (${CMAKE_ROOT}/Modules/FindVTK.cmake)

IF (USE_VTK_FILE)
  INCLUDE(${USE_VTK_FILE})
ENDIF (USE_VTK_FILE)

LINK_LIBRARIES( 
  vtkRendering
  vtkGraphics
  vtkImaging
  vtkHybrid
  vtkIO
  vtkFiltering
  vtkCommon
  vtkPatented
  vtkParallel
  vtkpng
  vtkjpeg
  vtkzlib
)

#
# Parte de uso de ITK
#


INCLUDE (${CMAKE_ROOT}/Modules/FindITK.cmake)
IF (USE_ITK_FILE)
  INCLUDE(${USE_ITK_FILE})
ENDIF (USE_ITK_FILE)
#LINK_DIRECTORIES(
#  ${ITK_BINARY_PATH}
#)
LINK_LIBRARIES(
  VXLNumerics
  ITKCommon
  ITKBasicFilters
  ITKIO
)

# General
IF (WIN32)
#	SET (CMAKE_CXX_FLAGS "/YX /GR /GX /Zm1000")
	SET (CMAKE_EXTRA_LINK_FLAGS "/nodefaultlib:\"msvcrtd.lib\"")
ENDIF(WIN32)

ADD_DEFINITIONS(-DTIXML_USE_STL)
SUBDIRS(tinyxml)

LINK_LIBRARIES(tinyxml)

# Fuentes
SET (PROJECT_SRC ${SkullyDoo_SOURCE_DIR}/src)
SET (PROJECT_BIN ${SkullyDoo_BINARY_DIR}/src)

INCLUDE_DIRECTORIES( 
  ${PROJECT_SRC}
  ${PROJECT_BIN}
)

SOURCE_GROUP(GUI "[.]fl$")
SOURCE_GROUP(MakeFile CMakeLists.txt)

AUX_SOURCE_DIRECTORY(common SkullyDoo_SRCS)
AUX_SOURCE_DIRECTORY(gui SkullyDoo_SRCS)
AUX_SOURCE_DIRECTORY(io SkullyDoo_SRCS)
AUX_SOURCE_DIRECTORY(main SkullyDoo_SRCS)
AUX_SOURCE_DIRECTORY(pipeline SkullyDoo_SRCS)
AUX_SOURCE_DIRECTORY(segmentation SkullyDoo_SRCS)
AUX_SOURCE_DIRECTORY(segmentation/activecontour SkullyDoo_SRCS)
AUX_SOURCE_DIRECTORY(segmentation/fastmarching SkullyDoo_SRCS)
AUX_SOURCE_DIRECTORY(segmentation/voxelgrow SkullyDoo_SRCS)

AUX_SOURCE_DIRECTORY(tcl SkullyDoo_SRCS)

SET(GUIBase_GUISRCS
  gui/AboutGUIBase.fl
  gui/DesktopGUIBase.fl
  gui/VolumeLoaderGUIBase.fl	
  gui/ProgressWindowGUIBase.fl
  gui/TCLFilterConfigGUIBase.fl
  gui/TCLFilterPipelineConfigGUIBase.fl
  gui/MetricsGUIBase.fl
)

IF (WIN32)
SET(SkullyDoo_SRCS  
  win32/Skully-Doo.rc
)
ENDIF (WIN32)


IF (WIN32)
	ADD_EXECUTABLE(SkullyDoo WIN32 SkullyDoo_SRCS)
ELSE (WIN32)
	ADD_EXECUTABLE(SkullyDoo SkullyDoo_SRCS)
ENDIF (WIN32)

FLTK_WRAP_UI(SkullyDoo GUIBase_GUISRCS)
