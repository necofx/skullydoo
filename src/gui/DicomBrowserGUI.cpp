#include "DicomBrowserGUI.h"

#include <itkImage.h>
#include <FL/Fl_File_Chooser.H>

DicomBrowserGUI::DicomBrowserGUI(const std::string &dir){
	filesNamesFinder = DICOMFileNames::New();
	dicomReader = DICOMIO::New();
	seriesReader = ImageSeriesReader::New();
	filesNamesFinder->SetDirectory(dir);
}

DicomBrowserGUI::~DicomBrowserGUI(void){
}

bool DicomBrowserGUI::exec(void){
	refreshBrowser();
	window->show();
	while(window->shown()){
		Fl::check();
	}
	return (bool)window->user_data();
}

void DicomBrowserGUI::refreshBrowser(void){
 	uIDs = filesNamesFinder->GetSeriesUIDs(true);
	btnLoad->deactivate();
	uIDsBrowser->clear();
	seriesFilesBrowser->clear();
	FileName uID;
	StringVector::iterator i;
	for (i=uIDs.begin();i!=uIDs.end();++i) {
		uID=(*i);
		uIDsBrowser->add(uID.c_str());
	}
}

void DicomBrowserGUI::changeDir(void){
	const char* dir=fl_dir_chooser("Change DICOM directory",NULL,0);
	if (!dir){
		refreshBrowser();	
		return;
	}
	filesNamesFinder->SetDirectory(dir);
	refreshBrowser();
}

void DicomBrowserGUI::seriesSelected(void){
	int sel=uIDsBrowser->value();
	if (sel) {
		seriesFilesBrowser->clear();
		btnLoad->activate();
		FileName selectedUID = uIDs[sel-1];
		seriesFilesNames = filesNamesFinder->GetFileNames(selectedUID);
		char lblFiles[15];
		sprintf(lblFiles, "%d files(s)", seriesFilesNames.size());
		StringVector::iterator i;
		FileName sName;
		for (i=seriesFilesNames.begin();i!=seriesFilesNames.end();++i){
			sName = (*i);
			sName.erase(0,sName.find_last_of("/")+1);
			seriesFilesBrowser->add(sName.c_str());
		}	
		seriesFilesBrowser->label(lblFiles);
		seriesFilesBrowser->redraw();
	}
}

void DicomBrowserGUI::loadFile(void){
	seriesReader->SetImageIO(dicomReader);
	seriesReader->SetFileNames(seriesFilesNames);
	try	{ 
		seriesReader->Update();
	}catch( itk::ExceptionObject & excp ){
		std::string msg = "Error reading series";
		fl_alert("DICOM Error:\n%s",msg.c_str());
	}
	image = seriesReader->GetOutput();
}

bool DicomBrowserGUI::reopen(void){
	return exec();
}

ImageModel::Pointer DicomBrowserGUI::readVolume(void){
	loadFile();
	ImageModel::Pointer output = ImageModel::New();
	output->setInputItkVolume(image);
    return output;	
}

void DicomBrowserGUI::sortBySliceLocation(){
	filesNamesFinder->SetFileNameSortingOrderToSortBySliceLocation();
}

void DicomBrowserGUI::sortByImageNumber(){
	filesNamesFinder->SetFileNameSortingOrderToSortByImageNumber();
}

void DicomBrowserGUI::sortByImagePositionPatient(){
	filesNamesFinder->SetFileNameSortingOrderToSortByImagePositionPatient ();
}
