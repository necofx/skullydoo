#include "DicomBrowserGUIBase.h"

#include "common/types.h"
#include "common/ImageModel.h"
#include "pipeline/ItkVtk.h"

#include <itkDicomImageIO.h>
#include <itkDICOMSeriesFileNames.h> 
#include <itkImageSeriesReader.h>
#include <itkImage.h>

#include <cstring>
#include <vector>

#pragma once

class DicomBrowserGUI :
	public DicomBrowserGUIBase
{
public:
	//crea la instancia que va a leer
	typedef itk::DicomImageIO			DICOMIO;
	typedef itk::DICOMSeriesFileNames	DICOMFileNames;
	typedef itk::ImageSeriesReader<ItkVtk::itkImageType>		ImageSeriesReader;
	
	DICOMFileNames::Pointer filesNamesFinder;
	DICOMIO::Pointer		dicomReader;
	ImageSeriesReader::Pointer	seriesReader;
	ItkVtk::itkImageType* image;

	DicomBrowserGUI(const std::string &dir);
	~DicomBrowserGUI(void);

	void refreshBrowser(void);
	virtual void changeDir(void);
	virtual void seriesSelected(void);
	virtual void loadFile(void);
	virtual ImageModel::Pointer readVolume(void);
	
	virtual void sortBySliceLocation();
	virtual void sortByImageNumber();
	virtual void sortByImagePositionPatient();

	bool exec(void);
	bool reopen(void);

protected:
	typedef std::string			  FileName;
	typedef std::vector<FileName> StringVector;

	StringVector uIDs;
	StringVector seriesFilesNames;
};
