/*
# $Id: ProgressWindowGUI.cpp,v 1.1 2003/05/02 22:21:53 sebasfiorent Exp $
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

#include "ProgressWindowGUI.h"
#include <FL/Enumerations.H>
#include <FL/fl_draw.H>
#include <itkCommand.h>
#include <vtkCallbackCommand.h>
#include <vector>

#include <time.h>


typedef std::vector<unsigned long> TagVector;
bool renderShown;

void observedDeleteCallback(vtkObject *caller, unsigned long event, void *clientdata, void *calldata){
	TagVector* tags=(TagVector*)clientdata;
	for (TagVector::iterator i=tags->begin();i!=tags->end();i++){
		caller->RemoveObserver(*i);
	}
	delete tags;
}

void vtkProgressCallBack(vtkObject *caller, unsigned long event, void *clientdata, void *calldata){
	vtkProcessObject* po=vtkProcessObject::SafeDownCast(caller);
	if (po) ProgressWindowGUI::Instance()->Update(vtkProcessObject::SafeDownCast(caller),event,calldata);
}

void vtkRenderCallBack(vtkObject *caller, unsigned long event, void *clientdata, void *calldata){
	vtkRenderWindow* rw=vtkRenderWindow::SafeDownCast(caller);
	if (rw) ProgressWindowGUI::Instance()->Update(rw,event);
}

void itkProgressCallBack(itk::Object* caller,const itk::EventObject& event,void* clientData){
	ProgressWindowGUI::Instance()->Update((itk::ProcessObject*)caller,event,clientData);
}

ProgressWindowGUI* ProgressWindowGUI::instance=0;
std::string ProgressWindowGUI::description="";

ProgressWindowGUI::ProgressWindowGUI(){
	progressBar->minimum(0);
	progressBar->maximum(100.0);
	log.open("progress.log",ios::out | ios::app | ios::ate);
}

ProgressWindowGUI* ProgressWindowGUI::Instance(){
	if (!instance) instance=new ProgressWindowGUI();
	return instance;
}

void ProgressWindowGUI::Observe(itk::ProcessObject* process,std::string msg,std::string instanceName){
	msgMap[process]=msg;
	iNamesMap[process]=instanceName;
	itk::CStyleCommand::Pointer cbc=itk::CStyleCommand::New();
	cbc->SetCallback(itkProgressCallBack);
	process->AddObserver(itk::StartEvent(), cbc);
	process->AddObserver(itk::ProgressEvent(),cbc);
	process->AddObserver(itk::EndEvent(), cbc);
}

void ProgressWindowGUI::Observe(vtkProcessObject* process,std::string msg,std::string instanceName){
	msgMap[process]=msg;
	iNamesMap[process]=instanceName;
	TagVector* tags=new TagVector();
	vtkCallbackCommand* cbc=vtkCallbackCommand::New();
	cbc->SetCallback(vtkProgressCallBack);
	tags->push_back(process->AddObserver(vtkCommand::StartEvent,cbc));
	tags->push_back(process->AddObserver(vtkCommand::EndEvent,cbc));
	tags->push_back(process->AddObserver(vtkCommand::ProgressEvent,cbc));
	cbc->Delete();
	
	cbc=vtkCallbackCommand::New();
	cbc->SetCallback(observedDeleteCallback);
	cbc->SetClientData(tags);
	tags->push_back(process->AddObserver(vtkCommand::DeleteEvent,cbc));
	cbc->Delete();
}

void ProgressWindowGUI::Observe(vtkRenderWindow* renwin){
	renderShown=false;
	TagVector* tags=new TagVector();
	vtkCallbackCommand* cbc=vtkCallbackCommand::New();
	cbc->SetCallback(vtkRenderCallBack);
	tags->push_back(renwin->AddObserver(vtkCommand::StartEvent,cbc));
	tags->push_back(renwin->AddObserver(vtkCommand::EndEvent,cbc));
	cbc->Delete();
	
	cbc=vtkCallbackCommand::New();
	cbc->SetCallback(observedDeleteCallback);
	cbc->SetClientData(tags);
	tags->push_back(renwin->AddObserver(vtkCommand::DeleteEvent,cbc));
	cbc->Delete();

}

void ProgressWindowGUI::doStartEvent(std::string msg){
	if (!window->shown()){
		window->show();
		fl_cursor(FL_CURSOR_WAIT);
	}
	statusText->label(msg.c_str());
	statusText->redraw();
	progressBar->value(0);
	Fl::check();
}

void ProgressWindowGUI::doProgressEvent(float amount){
	progressBar->value(amount*100.0);
	Fl::check();
}

void ProgressWindowGUI::doEndEvent(){
	progressBar->value(0);
	fl_cursor(FL_CURSOR_DEFAULT);
	window->hide();
	Fl::check();	
}

void ProgressWindowGUI::Update(itk::ProcessObject* caller,const itk::EventObject& event, void* clientData){
	std::string instancename=iNamesMap[caller];
	if (instancename=="") instancename="unknown";
	float amount;
	if (event.CheckEvent(&itk::StartEvent())){
		log << "Start ["<<(clock()*1000)/CLK_TCK<<"] "<<instancename << "->" <<msgMap[caller]<<"\n";
		log.flush();
		doStartEvent(msgMap[caller]);
	}
	else
	if (event.CheckEvent(&itk::ProgressEvent())){
		amount=caller->GetProgress();
		doProgressEvent(amount);
	}
	else
	if (event.CheckEvent(&itk::EndEvent())){
		log << "End ["<<(clock()*1000)/CLK_TCK<<"] "<<instancename << "->" <<msgMap[caller]<<"\n";
		log.flush();
		doEndEvent();
	}
}

void ProgressWindowGUI::Update(vtkProcessObject* caller,unsigned long event,void* callData){
	float amount;
	std::string instancename=iNamesMap[caller];
	if (instancename=="") instancename="unknown";
	switch(event) {
	case vtkCommand::ProgressEvent:
		if (!window->shown()) doStartEvent(msgMap[caller]);
		amount=*((float*)callData);
		doProgressEvent(amount);
		break;
	case vtkCommand::StartEvent:
		log << "Start ["<<(clock()*1000)/CLK_TCK<<"] "<<instancename << "->" <<msgMap[caller]<<"\n";
		log.flush();
		doStartEvent(msgMap[caller]);
		break;
	case vtkCommand::EndEvent:
		log << "End ["<<(clock()*1000)/CLK_TCK<<"] "<<instancename << "->" <<msgMap[caller]<<"\n";
		log.flush();
		doEndEvent();
	}
}

void RenderTimeOut(void* d){
	if (!renderShown){
		ProgressWindowGUI::Instance()->doStartEvent("Renderizando...");
		renderShown=true;
		Fl::check();
	}
}

void ProgressWindowGUI::Update(vtkRenderWindow* renwin,unsigned long event){
	switch(event) {
	case vtkCommand::StartEvent:
//		Fl::add_timeout(2.5,(Fl_Timeout_Handler)RenderTimeOut);
		break;
	case vtkCommand::EndEvent:
//		if (renderShown){
			//doEndEvent();
			//renderShown=false;
		//}
		break;
	}
}
