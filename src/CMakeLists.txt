# $Id: CMakeLists.txt,v 1.4 2004/06/07 23:21:27 nacholarrabide Exp $
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

# Makefile del codigo fuente


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

#
# Inclucion de librerias FLTK
#

INCLUDE (${CMAKE_ROOT}/Modules/FindFLTK.cmake)
INCLUDE_DIRECTORIES(
  ${FLTK_INCLUDE_PATH}
)
LINK_LIBRARIES(
  ${FLTK_LIBRARY}
)

#
# Inclucion  de libreria vtkFLTK
#

#INCLUDE(${CMAKE_ROOT}/Modules/FindvtkFLTK.cmake)

#INCLUDE_DIRECTORIES(
#  ${VTK_FLTK_INCLUDE_DIRS} 
#)

#LINK_LIBRARIES(
#  ${VTK_FLTK_LIBRARY_DIRS}
#)

IF (WIN32)
  LINK_LIBRARIES(wsock32)
ENDIF (WIN32)

#
# Inclucion librerias VTK
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
  vtkpng
  vtkjpeg
  vtkzlib
  vtkImagingTCL
  vtkGraphicsTCL
  vtkIOTCL
  vtkFilteringTCL 
  vtkCommonTCL  
  vtkPatentedTCL
  vtkHybridTCL
  vtkRenderingTCL
)

#
# Inclucion de librerias ITK
#


INCLUDE (${CMAKE_ROOT}/Modules/FindITK.cmake)
IF (USE_ITK_FILE)
  INCLUDE(${USE_ITK_FILE})
ENDIF (USE_ITK_FILE)

LINK_LIBRARIES(
  ITKCommon
  ITKBasicFilters
  ITKIO
)

# General
IF (WIN32)
	SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /YX /GR /GX /Zm1000")
	SET (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /nodefaultlib:\"library\"")
# msvcrtd.lib ")
ENDIF(WIN32)

ADD_DEFINITIONS(-DTIXML_USE_STL)
SUBDIRS(tinyxml)

LINK_LIBRARIES(tinyxml)


# Fuentes

INCLUDE_DIRECTORIES( 
  ${SkullyDoo_SOURCE_DIR}/src
  ${SkullyDoo_BINARY_DIR}/src
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
  gui/DicomBrowserGUIBase.fl
)

IF (WIN32)
  SET(SkullyDoo_SRCS  
    ${SkullyDoo_SRCS}
    win32/Skully-Doo.rc
  )
ENDIF (WIN32)

# Construyo el Ejecutable

IF (WIN32)
	ADD_EXECUTABLE(SkullyDoo WIN32 ${SkullyDoo_SRCS})
ELSE (WIN32)
	ADD_EXECUTABLE(SkullyDoo ${SkullyDoo_SRCS})
ENDIF (WIN32)

FLTK_WRAP_UI(SkullyDoo GUIBase_GUISRCS)
