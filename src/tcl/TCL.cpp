/*
# $Id: TCL.cpp,v 1.2 2003/05/23 19:18:59 sebasfiorent Exp $
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

#include "TCL.h"

#include <vtkTclUtil.h>
#include <vtkCallbackCommand.h>
#include <tk.h>
#include <map>
#include "TCLGlobals.h"
#include "gui/ProgressWindowGUI.h"
#include "GradientGrowthFilter.h"

extern "C" int Vtkcommontcl_Init(Tcl_Interp *interp);
extern "C" int Vtkfilteringtcl_Init(Tcl_Interp *interp);
extern "C" int Vtkimagingtcl_Init(Tcl_Interp *interp);
extern "C" int Vtkgraphicstcl_Init(Tcl_Interp *interp);
extern "C" int Vtkiotcl_Init(Tcl_Interp *interp);
extern "C" int Vtkrenderingtcl_Init(Tcl_Interp *interp);
extern "C" int Vtkpatentedtcl_Init(Tcl_Interp *interp);
extern "C" int Vtkhybridtcl_Init(Tcl_Interp *interp);

//Mapeo entre el long del tag, y el nombre de la variable que es borrada
//en el siguiente procedimiento.
//Las entradas se crean en TCL::setVtkVariable
unsigned long lastTag=0;
std::map<unsigned long,std::string> tagTonameMap;

void filterVtkVariableDeleteObject(ClientData cd){
  char temps[80];
  char *varname;
  Tcl_HashEntry *entry;
  int (*command)(ClientData, Tcl_Interp *,int, char *[]);
  vtkTclCommandArgStruct *as = (vtkTclCommandArgStruct *)cd;
  Tcl_Interp *interp = as->Interp;
  vtkTclInterpStruct *is = (vtkTclInterpStruct *)Tcl_GetAssocData(interp,(char *) "vtk",NULL);
  std::string tagvarname=tagTonameMap[as->Tag];
  char *realvarname=const_cast<char*>(tagvarname.c_str());
  tagTonameMap.erase(as->Tag);
  sprintf(temps,"%p",as->Pointer);
  entry = Tcl_FindHashEntry(&is->PointerLookup,temps); 
  if (entry){
	  varname= (char *)(Tcl_GetHashValue(entry));
	if (varname && tagvarname==varname){ //Si estoy borrando la variable que es dueña de la entrada en 
		Tcl_DeleteHashEntry(entry);	   //la tabla de punteros, borro la entrada
		if (varname) free(varname);
	}
  }
  entry = Tcl_FindHashEntry(&is->CommandLookup,realvarname);
  if (entry) Tcl_DeleteHashEntry(entry);
  entry = Tcl_FindHashEntry(&is->InstanceLookup,realvarname);
  if (entry) Tcl_DeleteHashEntry(entry);
}


void EmptyDelete_TCL_Command(ClientData cd){
}

//Comando ShowProgress
int TCL_ShowProgress(ClientData clientData,Tcl_Interp *interp,int argc,char *argv[]){
	char respuesta[1024];
	if (argc < 3){
		sprintf(_("Missing parameters. Use:\n %s Object Progress_Message"),argv[0]);
		Tcl_SetResult(interp, (char *) respuesta, TCL_VOLATILE);
		return TCL_ERROR;
	}
	std::string objname=argv[1];
	std::string mensaje=argv[2];
	TCL* tcl=TCL::Instance();
	vtkObject* vtkvar=tcl->getVtkVariable(objname);
	if (!vtkvar){
		sprintf(respuesta,_("The object %s doesn´t exist!"),objname.c_str());
		Tcl_SetResult(interp, (char *) respuesta, TCL_VOLATILE);
		return TCL_ERROR;
	}
	vtkProcessObject* vtkprocessobj=vtkProcessObject::SafeDownCast(vtkvar);
	if (!vtkprocessobj){
		sprintf(respuesta,_("The object %s isn´t a processing filter!"),objname.c_str());
		Tcl_SetResult(interp, (char *) respuesta, TCL_VOLATILE);
		return TCL_ERROR;
	}
	ProgressWindowGUI::Instance()->Observe(vtkprocessobj,mensaje,"unknownFromTCL");
	return TCL_OK;
}

//Comando NativeFilterSetParam
int TCL_NativeFilterSetParam(ClientData clientData,Tcl_Interp *interp,int argc,char *argv[]){
	char respuesta[1024];
	if (argc < 4){
		sprintf(respuesta,_("Missing parameters. Use:\n %s Filter Parameter Value"),argv[0]);
		Tcl_SetResult(interp, (char *) respuesta, TCL_VOLATILE);
		return TCL_ERROR;
	}
	std::string filtername=argv[1];
	std::string paramname=argv[2];
	std::string paramvalue=argv[3];
	TCLNativeFilter::Pointer filter;
	filter=TCLGlobals::NativeFiltersMap[filtername];
	if (!filter.GetPointer()){
		sprintf(respuesta,_("The native filter %s doesn´t exists!"),filtername.c_str());
		Tcl_SetResult(interp, (char *) respuesta, TCL_VOLATILE);
		return TCL_ERROR;
	}
	filter->setParameter(paramname,paramvalue);
	return TCL_OK;
}

//Comando NativeFilterGetParam
int TCL_NativeFilterGetParam(ClientData clientData,Tcl_Interp *interp,int argc,char *argv[]){
	char respuesta[1024];
	if (argc < 3){
		sprintf(respuesta,_("Missing parameters. Use:\n %s Filter Parameter"),argv[0]);
		Tcl_SetResult(interp, (char *) respuesta, TCL_VOLATILE);
		return TCL_ERROR;
	}
	std::string filtername=argv[1];
	std::string paramname=argv[2];
	TCLNativeFilter::Pointer filter;
	filter=TCLGlobals::NativeFiltersMap[filtername];
	if (!filter.GetPointer()){
		sprintf(respuesta,_("The native filter %s doesn´t exists!"),filtername.c_str());
		Tcl_SetResult(interp, (char *) respuesta, TCL_VOLATILE);
		return TCL_ERROR;
	}
	std::string paramvalue=filter->getParameter(paramname);
	Tcl_SetResult(interp, (char *) paramvalue.c_str(), TCL_VOLATILE);
	return TCL_OK;
}

//Comando NativeFilterClearParams
int TCL_NativeFilterClearParams(ClientData clientData,Tcl_Interp *interp,int argc,char *argv[]){
	char respuesta[1024];
	if (argc < 2){
		sprintf(respuesta,_("Missing parameters. Use:\n %s Filter"),argv[0]);
		Tcl_SetResult(interp, (char *) respuesta, TCL_VOLATILE);
		return TCL_ERROR;
	}
	std::string filtername=argv[1];
	TCLNativeFilter::Pointer filter;
	filter=TCLGlobals::NativeFiltersMap[filtername];
	if (!filter.GetPointer()){
		sprintf(respuesta,_("The native filter %s dosen´t exists!"),filtername.c_str());
		Tcl_SetResult(interp, (char *) respuesta, TCL_VOLATILE);
		return TCL_ERROR;
	}
	filter->clearParameters();
	return TCL_OK;
}

//Comando NativeFilterExecute
int TCL_NativeFilterExecute(ClientData clientData,Tcl_Interp *interp,int argc,char *argv[]){
	char respuesta[1024];
	if (argc < 2){
		sprintf(respuesta,_("Missing parameters. Use:\n %s Filter"),argv[0]);
		Tcl_SetResult(interp, (char *) respuesta, TCL_VOLATILE);
		return TCL_ERROR;
	}
	std::string filtername=argv[1];
	TCLNativeFilter::Pointer filter;
	filter=TCLGlobals::NativeFiltersMap[filtername];
	if (!filter.GetPointer()){
		sprintf(respuesta,_("The native filter %s dosen´t exists!"),filtername.c_str());
		Tcl_SetResult(interp, (char *) respuesta, TCL_VOLATILE);
		return TCL_ERROR;
	}
	filter->execute();
	return TCL_OK;
}



TCL* TCL::instance=0;

TCL::TCL(){
	interpreter=0;
	tempVarCount=0;
}

int TCL::InitializeTCLStub(Tcl_Interp* interp){
	Instance()->interpreter=interp;

	if (Tcl_Init(interp) == TCL_ERROR) {
		return TCL_ERROR;
	}
	if (Tk_Init(interp) == TCL_ERROR) {
		return TCL_ERROR;
	}
	if (Vtkcommontcl_Init(interp) == TCL_ERROR) {
		return TCL_ERROR;
    }
	if (Vtkfilteringtcl_Init(interp) == TCL_ERROR) {
		return TCL_ERROR;
    }
	if (Vtkimagingtcl_Init(interp) == TCL_ERROR) {
		return TCL_ERROR;
    }
	if (Vtkgraphicstcl_Init(interp) == TCL_ERROR) {
		return TCL_ERROR;
    }
	if (Vtkiotcl_Init(interp) == TCL_ERROR) {
		return TCL_ERROR;
    }
	if (Vtkrenderingtcl_Init(interp) == TCL_ERROR) {
		return TCL_ERROR;
    }
	if (Vtkpatentedtcl_Init(interp) == TCL_ERROR) {
		return TCL_ERROR;
    }
	if (Vtkhybridtcl_Init(interp) == TCL_ERROR) {
		return TCL_ERROR;
    }
	//Creo los comandos propios
	Tcl_CreateCommand(interp,(char*)TCLGlobals::COMMAND_SHOW_PROGRESS.c_str(),TCL_ShowProgress,0,(Tcl_CmdDeleteProc*)EmptyDelete_TCL_Command);
	Tcl_CreateCommand(interp,(char*)TCLGlobals::COMMAND_NATIVE_FILTER_GET_PARAM.c_str(),TCL_NativeFilterGetParam,0,(Tcl_CmdDeleteProc*)EmptyDelete_TCL_Command);
	Tcl_CreateCommand(interp,(char*)TCLGlobals::COMMAND_NATIVE_FILTER_SET_PARAM.c_str(),TCL_NativeFilterSetParam,0,(Tcl_CmdDeleteProc*)EmptyDelete_TCL_Command);
	Tcl_CreateCommand(interp,(char*)TCLGlobals::COMMAND_NATIVE_FILTER_CLEAR_PARAMS.c_str(),TCL_NativeFilterClearParams,0,(Tcl_CmdDeleteProc*)EmptyDelete_TCL_Command);
	Tcl_CreateCommand(interp,(char*)TCLGlobals::COMMAND_NATIVE_FILTER_EXECUTE.c_str(),TCL_NativeFilterExecute,0,(Tcl_CmdDeleteProc*)EmptyDelete_TCL_Command);
	//Hago el binding de los filtros nativos
	TCLGlobals::NativeFiltersMap["GradientGrowth"]=GradientGrowthFilter::New();
	//
	return TCL_OK;
}

TCL* TCL::Instance(){
	if (!instance) instance=new TCL();
	return instance;
}

Tcl_Interp* TCL::getInterpreter(){
	return interpreter;
}

bool TCL::existsVariable(std::string varname){
	bool found=false;
	char* name=const_cast<char*>(varname.c_str());
	if (Tcl_GetVar(getInterpreter(),name,0)) found=true;
	//Si no existe como variable normal, ver si es una variable vtk
	if (!found)	found=getVtkVariable(varname);
	return found;
}

std::string TCL::getVariable(std::string varname){
	char* name=const_cast<char*>(varname.c_str());
	char* varval=Tcl_GetVar(getInterpreter(),name,0);
	std::string result;
	if (varval) result=varval;
	return result;
}
	
void TCL::setVariable(std::string varname,std::string value){
	Tcl_SetVar(getInterpreter(),const_cast<char*>(varname.c_str()),const_cast<char*>(value.c_str()),0);
}

void TCL::setVtkVariable(std::string varname,vtkObject* variable){
	int is_new;
	char* name=const_cast<char*>(varname.c_str());
	Tcl_HashEntry *entry;
	if (!variable) return;
	//Comienzo
	vtkTclInterpStruct *is = (vtkTclInterpStruct *)Tcl_GetAssocData(getInterpreter(),(char *) "vtk",NULL);
	// 1. Si no existe la busqueda por puntero, creo la entrada
	char strpointer[80];
	sprintf(strpointer,"%p",variable);
	entry=Tcl_FindHashEntry(&is->PointerLookup,strpointer);
	if (!entry) {
		entry = Tcl_CreateHashEntry(&is->PointerLookup,strpointer,&is_new);
		Tcl_SetHashValue(entry,(ClientData)(strdup(name)));
	}
	// 2. Creo/Actualizo la entrada en la tabla de busqueda por nombre
	entry = Tcl_CreateHashEntry(&is->InstanceLookup,name,&is_new);
	if (entry) Tcl_SetHashValue(entry,(ClientData)(variable));
	// 3. Si ya existe un comando con este nombre, lo borro
	int (*command)(ClientData, Tcl_Interp *,int, char *[]);
	Tcl_CmdInfo cinf;
	char *tstr = strdup(variable->GetClassName());
	if (Tcl_GetCommandInfo(getInterpreter(),name,&cinf)){
		Tcl_DeleteCommand(getInterpreter(),name);
    }
	if (tstr) free(tstr);
	// 4. Creo la entrada en la tabla de busqueda de comandos comandos
	// Se busca la informacion del comando asociado respecto a la clase de la variable
	tstr = strdup(variable->GetClassName());
	if (Tcl_GetCommandInfo(getInterpreter(),tstr,&cinf)){
		if (cinf.clientData){
			vtkTclCommandStruct *cs = (vtkTclCommandStruct *)cinf.clientData;
			command = cs->CommandFunction;
		}
    }
	if (tstr) free(tstr);
	vtkTclCommandArgStruct *as = new vtkTclCommandArgStruct;
	as->Pointer = (void *)variable;
	as->Interp = getInterpreter();
	as->Tag = lastTag;
	tagTonameMap[as->Tag]=name;
	lastTag++;
	Tcl_CreateCommand(getInterpreter(),name,command,(ClientData)(as),(Tcl_CmdDeleteProc*)filterVtkVariableDeleteObject);
	entry = Tcl_CreateHashEntry(&is->CommandLookup,name,&is_new);
	if (entry) Tcl_SetHashValue(entry,(ClientData)command);
}

std::vector<std::string> TCL::getVtkObjectNamesInInterpreter(){
	std::vector<std::string> result;
	Tcl_HashEntry *entry;
	Tcl_HashSearch search;
	vtkTclInterpStruct *is = (vtkTclInterpStruct *)Tcl_GetAssocData(getInterpreter(),(char *) "vtk",NULL);
	entry = Tcl_FirstHashEntry(&is->InstanceLookup,&search);
	while (entry){
		std::string varname=(char *)(Tcl_GetHashKey(&is->InstanceLookup,entry));
		result.push_back(varname);
		entry = Tcl_NextHashEntry(&search);
	}
	return result;
}

void TCL::renameVtkVariable(std::string oldname,std::string newname){
	Tcl_HashEntry *entry;
	char* name=const_cast<char*>(oldname.c_str());
	vtkTclInterpStruct *is = (vtkTclInterpStruct *)Tcl_GetAssocData(getInterpreter(),(char *) "vtk",NULL);
	if ((entry = Tcl_FindHashEntry(&is->InstanceLookup,name))){
		ClientData variable=Tcl_GetHashValue(entry);
		Tcl_DeleteHashEntry(entry);
		entry=Tcl_FindHashEntry(&is->CommandLookup,name);
		int (*command)(ClientData, Tcl_Interp *,int, char *[]);
		command = (int (*)(ClientData,Tcl_Interp *,int,char *[]))Tcl_GetHashValue(entry);
		Tcl_DeleteHashEntry(entry);
		char* tstr = strdup(oldname.c_str());
		Tcl_CmdInfo cinf;
		if (Tcl_GetCommandInfo(getInterpreter(),tstr,&cinf)){
			if (cinf.clientData){
				vtkTclCommandArgStruct *cs = (vtkTclCommandArgStruct *)cinf.clientData;
				tagTonameMap[cs->Tag]=newname;
			}
		}
		if (tstr) free(tstr);
		/*char strpointer[80];
		sprintf(strpointer,"%p",variable);
		entry=Tcl_FindHashEntry(&is->PointerLookup,strpointer);
		char* hashname=(char*)Tcl_GetHashValue(entry);
		if (oldname==hashname){
			Tcl_DeleteHashEntry(entry);
			if (hashname) free(hashname);
			hashname=0;
		}
		*/
		int is_new;
		char* new_name=const_cast<char*>(newname.c_str());
		entry = Tcl_CreateHashEntry(&is->InstanceLookup,new_name,&is_new);
		Tcl_SetHashValue(entry,variable);
		/*
		if(!hashname){	//Significa que borré la entrada en la tabla de busq. por puntero
			entry = Tcl_CreateHashEntry(&is->PointerLookup,strpointer,&is_new);
			Tcl_SetHashValue(entry,(ClientData)(strdup(new_name)));
		}
		*/
		entry = Tcl_CreateHashEntry(&is->CommandLookup,new_name,&is_new);
		Tcl_SetHashValue(entry,(ClientData)command);
		bool ok=evaluate(std::string("rename ")+oldname+std::string(" ")+newname);
	}	
}

vtkObject* TCL::getVtkVariable(std::string varname){
	vtkObject* result=0;
	char* tstr = strdup(varname.c_str());
	Tcl_CmdInfo cinf;
	if (Tcl_GetCommandInfo(getInterpreter(),tstr,&cinf)){
		if (cinf.clientData){
			vtkTclCommandArgStruct *cs = (vtkTclCommandArgStruct *)cinf.clientData;
			result=(vtkObject*)cs->Pointer;
		}
	}
	if (tstr) free(tstr);
	return result;
}

void TCL::clearVariable(std::string varname){
	char *name=const_cast<char*>(varname.c_str());
	//Caso 1.Ver si es una variable vtk
	if (getVtkVariable(varname)!=0){
		Tcl_DeleteCommand(getInterpreter(),name);
	}
	else{
		Tcl_UnsetVar(getInterpreter(),name,0);
	}
}

bool TCL::evaluate(std::string code){
	lastError="";
	char* script=strdup(code.c_str());
	int ok;
	ok=Tcl_Eval(getInterpreter(),script);
	if (script) free(script);
	if (ok==TCL_ERROR) lastError=Tcl_GetStringResult(getInterpreter());			
	return (ok!=TCL_ERROR);
}

std::string TCL::getLastError(){
	return lastError;
}

std::string TCL::createNewFilterVariableName(){
	tempVarCount++;	
	char holder[255];
	sprintf(holder,"filterTemp%d",tempVarCount);
	return holder;
}
