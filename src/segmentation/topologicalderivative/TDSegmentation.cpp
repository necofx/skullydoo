#include "TDSegmentation.h"
#include "vtkImageCast.h"

#include "gui/ProgressWindowGUI.h"
#include "common/ImageModel.h"

TDSegmentation::TDSegmentation():SegmentationMethod(){
	this->direction = xy;
	rho=1;
}

TDSegmentation::~TDSegmentation(){
}	

void TDSegmentation::execute(ImageModel::Pointer vol,Segmentation::Pointer result){
	TDMethod* tder = new TDMethod();
	tder->SetInput(vol->getFilteredVtkVolume());

	tder->setClasses(classes);
	tder->setRho(rho);
	int activeDirections[3];
	//segmentate in the xy direction
	if (direction == xy){
		activeDirections[0] = activeDirections[1] = 1;
		activeDirections[2] = 0;
	//segmentate in the xz direction
	}else if (direction == xz){
		activeDirections[0] = activeDirections[2] = 1;
		activeDirections[1] = 0;
	//segmentate in the yz direction
	}else if (direction == yz){
		activeDirections[1] = activeDirections[2] = 1;
		activeDirections[0] = 0;
	//segmentate in 3 directions
	}else if (direction == td){
		activeDirections[0] = activeDirections[1] = activeDirections[2] = 1;
	}
	tder->setActiveDirections(activeDirections);
	tder->SetOutput(result->getImage()->getInputVtkVolume());
	ProgressWindowGUI::Instance()->Observe(tder,_("Executing Topological Derivative Segmentation"),vol->getLabel());
	tder->Update();

	result->setHaveImage(true);
	tder->Delete();
}

void TDSegmentation::setClasses(TDMethod::ClassesVector classes){
	this->classes=classes;
}

TDMethod::ClassesVector TDSegmentation::getClasses(){
	return classes;
}

void TDSegmentation::setDirection(TDSegmentation::SegmentationDirections dir){
	direction = dir;
}

TDSegmentation::SegmentationDirections TDSegmentation::getDirection(){
	return direction;
}

void TDSegmentation::setRho(double rho){
	this->rho = rho;
}

double TDSegmentation::getRho(){
	return rho;
}
