/*
# $Id: VoxelModel.h,v 1.1 2003/05/02 22:21:53 sebasfiorent Exp $
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

#ifndef	_VoxelModel_h_
#define _VoxelModel_h_

#include <string>
#include <vector>
#include <queue>
#include <stack>

class VoxelModel{
public:
	typedef std::vector<VoxelModel> Vector;
	typedef std::stack<VoxelModel> Stack;
	typedef std::queue<VoxelModel> Queue;
	int x;
	int y;
	int z;
	float intensity;

	VoxelModel();
	VoxelModel(const VoxelModel& other);
	VoxelModel& operator=(const VoxelModel& other);
};

#endif
