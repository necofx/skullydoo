/*
# $Id: TCLFilterPipelineConfigGUI.cpp,v 1.2 2003/05/23 19:18:58 sebasfiorent Exp $
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

#include "TCLFilterPipelineConfigGUI.h"
#include "TCLFilterConfigGUI.h"
#include "tcl/TCLFilterUtils.h"
#include "tcl/TCL.h"
#include <FL/fl_ask.H>
#include <algorithm>

#define BrowserFilterMASK "%s"

TCLFilterPipelineConfigGUI::TCLFilterPipelineConfigGUI(TCLFilterPipeline::Pointer filterpipeline,TCLFilter::Vector available):TCLFilterPipelineConfigGUIBase(){
	needReExecution=false;
	this->filterpipeline=filterpipeline;
	this->available=available;
	TCLFilter::Vector fv=filterpipeline->getFilters();
	for (TCLFilter::Vector::iterator i=fv.begin();i!=fv.end();i++){
		TCLFilter::Pointer f=(*i);
		browserFilterPipeline->add(f->getName().c_str(),f.GetPointer());
	}
	filterSelected();
	updateCorrectConfigurationStatus();
}

void TCLFilterPipelineConfigGUI::filterSelected(){
	int v=browserFilterPipeline->value();
	if (!v){
		updateAvailableFiltersCombo(filterpipeline->getInputType());
		btnRemove->deactivate();
		btnMoveEarlier->deactivate();
		btnMoveLater->deactivate();
		btnConfigure->deactivate();
	}
	else{
		TCLFilter::Pointer f=(TCLFilter*)browserFilterPipeline->data(v);
		updateAvailableFiltersCombo(f->getOutputType());
		btnRemove->activate();
		btnConfigure->activate();
		if (v==1) btnMoveEarlier->deactivate();
		else
			btnMoveEarlier->activate();
		if (v==browserFilterPipeline->size()) btnMoveLater->deactivate();
		else
			btnMoveLater->activate();
	}
}

void TCLFilterPipelineConfigGUI::updateCorrectConfigurationStatus(){
	//Recorro todos buscando que se cumplan los pares entrada/salida
	bool ok=true;
	std::string lastoutput=filterpipeline->getInputType();
	int v=1;
	while (ok && v<=browserFilterPipeline->size()){
		TCLFilter::Pointer f=(TCLFilter*)browserFilterPipeline->data(v);
		ok=(f->getInputType()==lastoutput);
		lastoutput=f->getOutputType();
		v++;
	}
	if (ok) ok=(filterpipeline->getOutputType()==lastoutput);
	if (ok)
		btnCerrar->activate();
	else
		btnCerrar->deactivate();
}

void TCLFilterPipelineConfigGUI::updateAvailableFiltersCombo(std::string inputtype){
	TCLFilter::Vector fl=TCLFilterUtils::hasInputType(inputtype,available);
	browserAvailableFilters->clear();
	bool added=false;
	for (TCLFilter::Vector::iterator i=fl.begin();i!=fl.end();i++){
		TCLFilter::Pointer f=(*i);
		browserAvailableFilters->add(f->getName().c_str(),f.GetPointer());
		added=true;
	}
	if (added){
		btnAdd->activate();
		browserAvailableFilters->activate();
	}
	else{
		btnAdd->deactivate();
		browserAvailableFilters->deactivate();
	}
}

TCLFilter::Pointer TCLFilterPipelineConfigGUI::getSelectedFilter(){
	filterSelected();
	int v=browserFilterPipeline->value();
	if (!v) return 0;
	TCLFilter::Pointer f=(TCLFilter*)browserFilterPipeline->data(v);
	return f;
}
	
	
void TCLFilterPipelineConfigGUI::availableFilterSelected(){

}
	
void TCLFilterPipelineConfigGUI::addFilter(){
	//Creo un clon
	if (!browserAvailableFilters->value()) return;
	TCLFilter::Pointer selected=(TCLFilter*)browserAvailableFilters->data(browserAvailableFilters->value());
	TCLFilter::Pointer f=TCLFilter::New();
	f->setFileName(selected->getFileName());
	bool ok=f->initialize();
	if (!ok){
		fl_alert(_("Failed to initialize filter. Error:\n [%s]"),f->getTCLError().c_str());
		return;
	}
	filters.push_back(f);
	char holder[1024];
	sprintf(holder,BrowserFilterMASK,f->getName().c_str(),f->getOutputType().c_str());
	int v=browserFilterPipeline->value()+1;
	browserFilterPipeline->insert(v,holder,f.GetPointer());
	browserFilterPipeline->value(v);
	needReExecution=true;
	filterSelected();
	updateAvailableFiltersCombo(f->getOutputType());
	updateCorrectConfigurationStatus();
}

void TCLFilterPipelineConfigGUI::removeFilter(){
	TCLFilter::Pointer f=getSelectedFilter();
	if (!f.GetPointer()) return;
	f->finalize();
	int v=browserFilterPipeline->value();
	browserFilterPipeline->remove(v);
	TCLFilter::Vector::iterator i=std::find(filters.begin(),filters.end(),f);
	if (i!=filters.end()) filters.erase(i);
	browserFilterPipeline->value(v);
	needReExecution=true;
	filterSelected();
	updateCorrectConfigurationStatus();
}

void TCLFilterPipelineConfigGUI::moveEarlier(){
	TCLFilter::Pointer f=getSelectedFilter();
	if (!f.GetPointer()) return;
	int v=browserFilterPipeline->value();
	browserFilterPipeline->move(v-1,v);
	needReExecution=true;
	updateCorrectConfigurationStatus();
}

void TCLFilterPipelineConfigGUI::moveLater(){
	TCLFilter::Pointer f=getSelectedFilter();
	if (!f.GetPointer()) return;
	int v=browserFilterPipeline->value();
	browserFilterPipeline->move(v+1,v);
	needReExecution=true;
	updateCorrectConfigurationStatus();
}

void TCLFilterPipelineConfigGUI::configureFilter(){
	TCLFilter::Pointer f=getSelectedFilter();
	if (!f.GetPointer()) return;
	TCLFilterConfigGUI* cfg=new TCLFilterConfigGUI(f);
	if (cfg->configure()) needReExecution=true;
	delete cfg;
}

bool TCLFilterPipelineConfigGUI::configure(){
	filterpipeline->enablePredefinedObjects();
	window->show();
	while (window->shown()){
		Fl::check();
	}
	bool ok=true;
	TCLFilter::Vector fv;
	for (int v=1;v<=browserFilterPipeline->size();v++){
		TCLFilter::Pointer f=(TCLFilter*)browserFilterPipeline->data(v);
		fv.push_back(f);
	}
	filterpipeline->setFilters(fv);
	filterpipeline->disablePredefinedObjects();
	if (needReExecution){
		TCLFilterPipeline::ExecStatus status=filterpipeline->execute();
		if (status==TCLFilterPipeline::EXEC_ERROR){
			fl_alert(_("TCL Error:\n%s"),filterpipeline->getErrorMessage().c_str());
			ok=false;
		}
		if (status==TCLFilterPipeline::EXEC_MISSED_OUTPUT){
			fl_alert(_("Expected output with type %s. Check scripts (filter_output variable)"),filterpipeline->getOutputType().c_str());
			ok=false;
		}
	}
	filterpipeline->enablePredefinedObjects();
	filters.clear();
	filterpipeline->disablePredefinedObjects();
	return ok;
}
