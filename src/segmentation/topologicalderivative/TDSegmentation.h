#ifndef _TDSegmentation_h_
#define _TDSegmentation_h_
#ifdef _MSC_VER
	#pragma warning  ( disable : 4786 )
#endif

#include "segmentation/SegmentationMethod.h"
#include "TDMethod.h"

class TDSegmentation:public SegmentationMethod{
public:
	enum SegmentationDirections
		 {xy=0, xz=1, yz=2, td=3};

protected:
	TDSegmentation::SegmentationDirections direction;
	double rho;
	int activeDirections[3];


	virtual void execute(ImageModel::Pointer vol,Segmentation::Pointer result);
	TDMethod::ClassesVector classes;

public:
	NewMacro(TDSegmentation);
	virtual ~TDSegmentation();

	void setClasses(TDMethod::ClassesVector classes);
	TDMethod::ClassesVector getClasses();
	void setDirection(TDSegmentation::SegmentationDirections direction);
	TDSegmentation::SegmentationDirections getDirection();
	void setRho(double rho);
	double getRho();
};

#endif