/*
# $Id: TCLFilter.h,v 1.1 2003/05/02 22:21:59 sebasfiorent Exp $
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

#if !defined(_TCLFILTER_H_)
#define _TCLFILTER_H_

#include "common/Object.h"
#include "tinyxml/tinyxml.h"
#include "TCLFilterParameter.h"

#include <vector>
#include <map>
#include <string>

class TCLFilter : public Object{
protected:
	std::string filename;
	std::string executionError;
	TCLFilterParameter::Vector parameters;
	TiXmlDocument dom;
	TiXmlElement* root;
	bool finalized;
	bool initialized;

	typedef std::map<std::string,std::string> VarsMapType;
	VarsMapType tclVarsMap;

	void loadParameters();
public:
	NewMacro(TCLFilter);
	virtual ~TCLFilter();
	
	typedef std::vector<Pointer> Vector;
	void setFileName(std::string filename);
	

	std::string getFileName();
	std::string getInitTclCode();
	std::string getTclMarquedCode();
	std::string getTclParsedCode();
	std::string getFinishTclCode();
	std::string getInputType();
	std::string getOutputType();
	std::string getName();
	TCLFilterParameter::Vector getParameters();
	bool existParameter(std::string name);
	TCLFilterParameter::Pointer getParameter(std::string name);
	std::string getMappedName(std::string varname);
	/*
	 *	Ejecuta el código de inicialización TCL, y almacena
	 * en tclVarsMap los nombres de los objetos creados
	 * en esta etapa para ser utilizados en la posterior ejecucion.
	 * True si es exitoso, Falso de lo contrario
	 */
	bool initialize();

	/*
	 *	Ejecuta el filtro de acuerdo a los valores configurados
	 */
	bool execute();

	void finalize();

	std::string getTCLError();
};

#endif // !defined(_TCLFILTER_H_)
