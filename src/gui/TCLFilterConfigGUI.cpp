/*
# $Id: TCLFilterConfigGUI.cpp,v 1.1 2003/05/02 22:21:53 sebasfiorent Exp $
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

#include "TCLFilterConfigGUI.h"

#include <FL/Fl_Float_Input.H>
#include <FL/Fl_Int_Input.H>
#include <FL/Fl_Value_Slider.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Box.H>

#define WIDGET_HEIGHT 25

TCLFilterConfigGUI::TCLFilterConfigGUI(TCLFilter::Pointer filter){
	this->filter=filter;
	window->label(makeLabel(filter->getName()));
	buildEditWidgets();
}

TCLFilterConfigGUI::~TCLFilterConfigGUI(){
	for (std::vector<char*>::iterator i=labels.begin();i!=labels.end();i++){
		char* l=(*i);
		if (l) free(l);
	}
}

char* TCLFilterConfigGUI::makeLabel(std::string l){
	char* f=strdup(l.c_str());
	labels.push_back(f);
	return f;
}

void TCLFilterConfigGUI::buildEditWidget(TCLFilterContinuousParameter::Pointer parameter){
	Fl_Widget* w;
	std::string rmin=parameter->getRangeMin();
	std::string rmax=parameter->getRangeMax();
	bool isfloat=(parameter->getParamType()==TCLFilterContinuousParameter::FloatParam);
	if (rmin!="" && rmax!=""){
		Fl_Value_Slider* vs=new Fl_Value_Slider(0,0,WIDGET_HEIGHT,WIDGET_HEIGHT);
		vs->type(FL_HOR_NICE_SLIDER);
		if (isfloat){
			vs->range(atof(rmin.c_str()),atof(rmax.c_str()));
			vs->step(0.1);
		}
		else{
			vs->range(atoi(rmin.c_str()),atoi(rmax.c_str()));	
			vs->step(1.0);
		}
		if (parameter->valueAssigned()){
			if (isfloat) vs->value(atof(parameter->getValue().c_str()));
			else
				vs->value(atoi(parameter->getValue().c_str()));
		}
		else
			vs->value(vs->minimum());
		w=vs;
	}
	else{
		Fl_Input* vi;
		if (isfloat) vi=new Fl_Float_Input(0,0,WIDGET_HEIGHT,WIDGET_HEIGHT);
		else
			vi=new Fl_Int_Input(0,0,WIDGET_HEIGHT,WIDGET_HEIGHT);	
		if (parameter->valueAssigned())	vi->value(parameter->getValue().c_str());
		else
			vi->value("0");
		w=vi;
	}
	w->user_data(parameter.GetPointer());
	w->tooltip(makeLabel(parameter->getHint()));
	packEditors->add(w);
	widContinuous.push_back(w);
	Fl_Box* label=new Fl_Box(0,0,WIDGET_HEIGHT,WIDGET_HEIGHT,makeLabel(parameter->getName()));
	label->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE | FL_ALIGN_WRAP);
	label->labelsize(10);
	packLabels->add(label);
}

void TCLFilterConfigGUI::buildEditWidget(TCLFilterDiscreteParameter::Pointer parameter){
	Fl_Choice* fc=new Fl_Choice(0,0,WIDGET_HEIGHT,WIDGET_HEIGHT);
	std::vector<std::string> values=parameter->getValues();
	int pos=0;
	int selected=0;
	std::string val=parameter->getValue(); 
	for (std::vector<std::string>::iterator i=values.begin();i!=values.end();i++){
		std::string v=(*i);
		if (v==val) selected=pos;
		fc->add(v.c_str());
		pos++;
	}
	fc->value(selected);
	fc->user_data(parameter.GetPointer());
	fc->tooltip(makeLabel(parameter->getHint()));
	packEditors->add(fc);
	widDiscrete.push_back(fc);
	Fl_Box* label=new Fl_Box(0,0,WIDGET_HEIGHT,WIDGET_HEIGHT,makeLabel(parameter->getName()));
	label->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE | FL_ALIGN_WRAP);
	label->labelsize(10);
	packLabels->add(label);
}

void TCLFilterConfigGUI::buildEditWidget(TCLFilterStringParameter::Pointer parameter){
	Fl_Input* inp=new Fl_Input(0,0,WIDGET_HEIGHT,WIDGET_HEIGHT);
	inp->value(parameter->getValue().c_str());
	inp->user_data(parameter.GetPointer());
	inp->tooltip(makeLabel(parameter->getHint()));
	packEditors->add(inp);
	widString.push_back(inp);
	Fl_Box* label=new Fl_Box(0,0,WIDGET_HEIGHT,WIDGET_HEIGHT,makeLabel(parameter->getName()));
	label->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE | FL_ALIGN_WRAP);
	label->labelsize(10);
	packLabels->add(label);
}


void TCLFilterConfigGUI::buildEditWidgets(){
	TCLFilterParameter::Vector params=filter->getParameters();
	for (TCLFilterParameter::Vector::iterator i=params.begin();i!=params.end();i++){
		TCLFilterParameter::Pointer fp=(*i);
		if (fp->getType()=="Discrete_Parameter"){
			TCLFilterDiscreteParameter::Pointer dp=(TCLFilterDiscreteParameter*)fp.GetPointer();
			buildEditWidget(dp);
		}
		else
		if (fp->getType()=="Continuous_Parameter"){
			TCLFilterContinuousParameter::Pointer cp=(TCLFilterContinuousParameter*)fp.GetPointer();
			buildEditWidget(cp);
		}
		else
		if (fp->getType()=="String_Parameter"){
			TCLFilterStringParameter::Pointer sp=(TCLFilterStringParameter*)fp.GetPointer();
			buildEditWidget(sp);
		}
	}
	packLabels->spacing(5);
	packLabels->redraw();
	packEditors->spacing(5);
	packEditors->redraw();
}

bool TCLFilterConfigGUI::configure(){
	window->show();
	while(window->shown()){
		Fl::check();
	}
	bool ok=(bool)window->user_data();
	if (ok){
		WidgetVector::iterator i;
		for (i=widContinuous.begin();i!=widContinuous.end();i++){
			Fl_Widget* w=(Fl_Widget*)*i;
			TCLFilterContinuousParameter::Pointer cp=(TCLFilterContinuousParameter*)w->user_data();
			std::string rmin=cp->getRangeMin();
			std::string rmax=cp->getRangeMax();
			std::string val="";
			if (rmin!="" && rmax!=""){
				char holder[255];
				if (cp->getParamType()==TCLFilterContinuousParameter::FloatParam) sprintf(holder,"%4.2f",(float)((Fl_Value_Slider*)w)->value());
				else 
					sprintf(holder,"%d",(int)((Fl_Value_Slider*)w)->value());
				val=holder;
			}
			else
				val=((Fl_Input*)w)->value();	
			cp->setValue(val);
		}
		for (i=widDiscrete.begin();i!=widDiscrete.end();i++){
			Fl_Choice* w=(Fl_Choice*)(*i);
			TCLFilterDiscreteParameter::Pointer dp=(TCLFilterDiscreteParameter*)w->user_data();
			dp->setValue(w->text());
		}
		for (i=widString.begin();i!=widString.end();i++){
			Fl_Input* w=(Fl_Input*)(*i);
			TCLFilterStringParameter::Pointer sp=(TCLFilterStringParameter*)w->user_data();
			sp->setValue(w->value());
		}
	}
	return ok;
}
